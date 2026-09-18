#ifndef DETECTOR_HPP__
#define DETECTOR_HPP__
#include "Audio/AudioDevice.hpp"
#include <QScopedArrayPointer>
#include "commons.h"

//
// output device that distributes data in predefined chunks via a signal
//
// the underlying device for this abstraction is just the buffer that
// stores samples throughout a receiving period
//
class Detector : public AudioDevice
{
  Q_OBJECT;

public:
  //
  // we down sample by a factor of 4
  //
  // the samplesPerFFT argument is the number after down sampling
  //
  // sink is the dec_data buffer this detector fills; passing nullptr uses the
  // global dec_data shared with the Fortran decoder. A second audio source
  // needs its own buffer, otherwise both detectors interleave samples into the
  // same one and corrupt each other.
  //
  Detector (unsigned frameRate, double periodLengthInSeconds, unsigned downSampleFactor = 4u,
            dec_data_t * sink = nullptr, QObject * parent = 0);

  void setTRPeriod(double p) {m_period=p;}
  bool reset () override;

  Q_SIGNAL void framesWritten (qint64) const;
  // Emitted only when samples actually arrive, so a silent or dead source
  // simply stops reporting rather than leaving a stale reading behind.
  Q_SIGNAL void level (float db, float dbmax) const;
  Q_SLOT void setBlockSize (unsigned);

protected:
  qint64 readData (char * /* data */, qint64 /* maxSize */) override
  {
    return -1;			// we don't produce data
  }

  qint64 writeData (char const * data, qint64 maxSize) override;

private:
  void clear ();		// discard buffer contents

  dec_data_t * m_sink;
  unsigned m_frameRate;
  double   m_period;
  unsigned m_downSampleFactor;
  unsigned m_mstr0;             // per instance: was a function static, which
                                // two detectors would have shared
  qint32 m_samplesPerFFT;	// after any down sampling
  static size_t const max_buffer_size {7 * 512};
  QScopedArrayPointer<short> m_buffer; // de-interleaved sample buffer
  // big enough for all the
  // samples for one increment of
  // data (a signals worth) at
  // the input sample rate
  unsigned m_bufferPos;
};

#endif
