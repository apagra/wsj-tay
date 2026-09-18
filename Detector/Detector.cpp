#include "Detector.hpp"
#include <QDateTime>
#include <QtAlgorithms>
#include <QDebug>
#include <math.h>
#include "commons.h"

#include "moc_Detector.cpp"

extern "C" {
  void   fil4_(qint16*, qint32*, qint16*, qint32*);
}

extern dec_data_t dec_data;

Detector::Detector (unsigned frameRate, double periodLengthInSeconds,
                    unsigned downSampleFactor, dec_data_t * sink, QObject * parent)
  : AudioDevice (parent)
  , m_sink (sink ? sink : &dec_data)
  , m_frameRate (frameRate)
  , m_period (periodLengthInSeconds)
  , m_downSampleFactor (downSampleFactor)
  , m_mstr0 (999999)
  , m_samplesPerFFT {max_buffer_size}
  , m_buffer ((downSampleFactor > 1) ?
              new short [max_buffer_size * downSampleFactor] : nullptr)
  , m_bufferPos (0)
{
  (void)m_frameRate;            // quell compiler warning
  clear ();
}

void Detector::setBlockSize (unsigned n)
{
  m_samplesPerFFT = n;
}

bool Detector::reset ()
{
  clear ();
  // don't call base class reset because it calls seek(0) which causes
  // a warning
  return isOpen ();
}

void Detector::clear ()
{
  // set index to roughly where we are in time (1ms resolution)
  // qint64 now (QDateTime::currentMSecsSinceEpoch ());
  // unsigned msInPeriod ((now % 86400000LL) % (m_period * 1000));
  // m_sink->params.kin = qMin ((msInPeriod * m_frameRate) / 1000, static_cast<unsigned> (sizeof (m_sink->d2) / sizeof (m_sink->d2[0])));
  m_sink->params.kin = 0;
  m_bufferPos = 0;

  // fill buffer with zeros (G4WJS commented out because it might cause decoder hangs)
  // qFill (m_sink->d2, m_sink->d2 + sizeof (m_sink->d2) / sizeof (m_sink->d2[0]), 0);
}

qint64 Detector::writeData (char const * data, qint64 maxSize)
{
  
  qint64 ms0 = QDateTime::currentMSecsSinceEpoch() % 86400000;
  unsigned mstr = ms0 % int(1000.0*m_period); // ms into the nominal Tx start time
  if(mstr < m_mstr0) {              //When mstr has wrapped around to 0, restart the buffer
    m_sink->params.kin = 0;
    m_bufferPos = 0;
  }
  m_mstr0=mstr;

  // Report the level of this batch, using the same quantities symspec.f90
  // computes for the first source so the two can be compared directly.
  {
    auto const * samples = reinterpret_cast<qint16 const *> (data);
    auto const count = maxSize / static_cast<qint64> (sizeof (qint16));
    double sq = 0.;
    int pk = 0;
    for (qint64 i = 0; i < count; ++i)
      {
        int const v = samples[i];
        sq += double (v) * v;
        if (std::abs (v) > pk) pk = std::abs (v);
      }
    if (count > 0)
      {
        Q_EMIT level (sq > 0. ? 10. * std::log10 (sq / count) : 0.,
                      pk > 0 ? 20. * std::log10 (double (pk)) : 0.);
      }
  }

  // no torn frames
  Q_ASSERT (!(maxSize % static_cast<qint64> (bytesPerFrame ())));
  // these are in terms of input frames (not down sampled)
  size_t framesAcceptable ((sizeof (m_sink->d2) /
                            sizeof (m_sink->d2[0]) - m_sink->params.kin) * m_downSampleFactor);
  size_t framesAccepted (qMin (static_cast<size_t> (maxSize /
                                                    bytesPerFrame ()), framesAcceptable));

  if (framesAccepted < static_cast<size_t> (maxSize / bytesPerFrame ())) {
    qDebug () << "dropped " << maxSize / bytesPerFrame () - framesAccepted
                << " frames of data on the floor!"
                << m_sink->params.kin << mstr;
    }

    for (unsigned remaining = framesAccepted; remaining; ) {
      size_t numFramesProcessed (qMin (m_samplesPerFFT *
                                       m_downSampleFactor - m_bufferPos, remaining));

      if(m_downSampleFactor > 1) {
        store (&data[(framesAccepted - remaining) * bytesPerFrame ()],
               numFramesProcessed, &m_buffer[m_bufferPos]);
        m_bufferPos += numFramesProcessed;

        if(m_bufferPos==m_samplesPerFFT*m_downSampleFactor) {
          qint32 framesToProcess (m_samplesPerFFT * m_downSampleFactor);
          qint32 framesAfterDownSample (m_samplesPerFFT);
          if(m_downSampleFactor > 1 && m_sink->params.kin>=0 &&
             m_sink->params.kin < (NTMAX*12000 - framesAfterDownSample)) {
            fil4_(&m_buffer[0], &framesToProcess, &m_sink->d2[m_sink->params.kin],
                  &framesAfterDownSample);
            m_sink->params.kin += framesAfterDownSample;
          } else {
            // qDebug() << "framesToProcess     = " << framesToProcess;
            // qDebug() << "m_sink->params.kin = " << m_sink->params.kin;
            // qDebug() << "secondInPeriod      = " << secondInPeriod();
            // qDebug() << "framesAfterDownSample" << framesAfterDownSample;
          }
          Q_EMIT framesWritten (m_sink->params.kin);
          m_bufferPos = 0;
        }

      } else {
        store (&data[(framesAccepted - remaining) * bytesPerFrame ()],
               numFramesProcessed, &m_sink->d2[m_sink->params.kin]);
        m_bufferPos += numFramesProcessed;
        m_sink->params.kin += numFramesProcessed;
        if (m_bufferPos == static_cast<unsigned> (m_samplesPerFFT)) {
          Q_EMIT framesWritten (m_sink->params.kin);
          m_bufferPos = 0;
        }
      }
      remaining -= numFramesProcessed;
    }

    // we drop any data past the end of the buffer on the floor until
    // the next period starts
    return maxSize;
}
