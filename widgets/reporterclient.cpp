#include "reporterclient.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QTimer>
#include <QUrl>
#include <QWebSocket>

namespace
{
  // Cloudflare closes a connection that says nothing for a while; "ping" is
  // answered by the server's front door without waking it, so it costs nothing.
  int const PING_MS = 30000;
  // The server refuses reports closer together than 0.4 s. Two passes can
  // finish within milliseconds of each other, so the second waits its turn.
  int const SEND_GAP_MS = 450;
  int const RETRY_FIRST_MS = 2000;
  int const RETRY_LAST_MS = 60000;

  // Six digits read out as two threes, "482-913": easy over a telephone or on
  // the air, and the server takes it with or without the dash.
  QString make_code ()
  {
    auto const n = QRandomGenerator::system ()->bounded (1000000);
    auto const digits = QString {"%1"}.arg (n, 6, 10, QChar {'0'});
    return digits.left (3) + '-' + digits.mid (3);
  }

  QString clean_code (QString const& code)
  {
    QString out;
    for (auto const c : code.toUpper ()) { if (c.isLetterOrNumber ()) out += c; }
    return out;
  }

  QUrl socket_url (QString base)
  {
    if (base.startsWith ("https://")) base.replace (0, 5, "wss");
    else if (base.startsWith ("http://")) base.replace (0, 4, "ws");
    return QUrl {base + "/ws"};
  }
}

ReporterClient::ReporterClient (QObject * parent)
  : QObject {parent}
  , socket_ {new QWebSocket {QString {}, QWebSocketProtocol::VersionLatest, this}}
  , reconnect_timer_ {new QTimer {this}}
  , ping_timer_ {new QTimer {this}}
  , send_timer_ {new QTimer {this}}
  , code_ {make_code ()}
  , retry_ms_ {RETRY_FIRST_MS}
{
  reconnect_timer_->setSingleShot (true);
  send_timer_->setSingleShot (true);
  ping_timer_->setInterval (PING_MS);
  connect (reconnect_timer_, &QTimer::timeout, this, &ReporterClient::open);
  connect (send_timer_, &QTimer::timeout, this, &ReporterClient::flush_queue);
  connect (ping_timer_, &QTimer::timeout, this, [this] () {
      if (connected_) socket_->sendTextMessage ("ping");
    });

  connect (socket_, &QWebSocket::connected, this, [this] () {
      connected_ = true;
      retry_ms_ = RETRY_FIRST_MS;
      said_trouble_ = false;
      ping_timer_->start ();
      say_hello ();
      if (!following_.isEmpty ()) send_watch ();
    });
  connect (socket_, &QWebSocket::disconnected, this, [this] () {
      auto const was = connected_;
      connected_ = false;
      ping_timer_->stop ();
      send_timer_->stop ();
      queue_.clear ();
      if (base_.isEmpty ()) return;
      // Said once, not on every attempt: a server that is not there is news,
      // a server that is still not there is not.
      if (!said_trouble_)
        {
          said_trouble_ = true;
          Q_EMIT trouble (was ? tr ("MY_NET: connection lost, trying again")
                              : tr ("MY_NET: %1").arg (socket_->errorString ()));
        }
      reconnect_timer_->start (retry_ms_);
      retry_ms_ = qMin (retry_ms_ * 2, RETRY_LAST_MS);
    });
  connect (socket_, &QWebSocket::textMessageReceived, this, &ReporterClient::on_message);
}

ReporterClient::~ReporterClient ()
{
  // Closed on purpose, so the disconnect handler has nothing to retry.
  base_.clear ();
  socket_->close ();
}

void ReporterClient::set_base_url (QString const& url)
{
  auto base = url.trimmed ();
  while (base.endsWith ('/')) base.chop (1);
  if (base == base_) return;
  base_ = base;
  reconnect_timer_->stop ();
  retry_ms_ = RETRY_FIRST_MS;
  said_trouble_ = false;
  if (socket_->state () != QAbstractSocket::UnconnectedState)
    {
      // Closing reconnects to the new address by itself, through the
      // disconnect handler; with no address it stays closed.
      socket_->close ();
      return;
    }
  open ();
}

void ReporterClient::open ()
{
  if (base_.isEmpty () || call_.isEmpty ()) return;
  if (socket_->state () != QAbstractSocket::UnconnectedState) return;
  socket_->open (socket_url (base_));
}

void ReporterClient::set_identity (QString const& call, QString const& grid,
                                   QString const& version)
{
  auto const changed = call.trimmed ().toUpper () != call_ || grid != grid_;
  call_ = call.trimmed ().toUpper ();
  grid_ = grid;
  version_ = version;
  if (connected_ && changed) say_hello ();
  else if (!connected_) open ();
}

void ReporterClient::set_sending (bool on)
{
  sending_ = on;
}

QString ReporterClient::code_for (QString const& call) const
{
  auto const who = call.trimmed ().toUpper ();
  if (who == call_) return code_;
  return codes_.value (who);
}

void ReporterClient::say_hello ()
{
  QJsonObject o;
  o["t"] = "hello";
  o["call"] = call_;
  o["grid"] = grid_;
  o["version"] = version_;
  o["code"] = code_;
  send (o);
}

void ReporterClient::send_watch ()
{
  QJsonObject o;
  o["t"] = "watch";
  o["call"] = following_;
  o["code"] = following_code_;
  if (!last_period_.isEmpty ()) o["since"] = last_period_;
  send (o);
}

void ReporterClient::send (QJsonObject const& o)
{
  if (!connected_) return;
  socket_->sendTextMessage (QString::fromUtf8 (QJsonDocument {o}.toJson (QJsonDocument::Compact)));
}

void ReporterClient::follow (QString const& call, QString const& code)
{
  following_ = call.trimmed ().toUpper ();
  following_code_ = code.trimmed ().isEmpty () ? code_for (following_) : clean_code (code);
  if (!following_.isEmpty () && !following_code_.isEmpty ()) codes_[following_] = following_code_;
  following_present_ = true;
  follow_dial_ = 0;
  follow_band_.clear ();
  follow_mode_.clear ();
  last_period_.clear ();
  last_count_ = 0;
  send_watch ();
}

void ReporterClient::report_period (QString const& call, QString const& grid,
                                    QString const& band, QString const& mode, int dial,
                                    QString const& version, QString const& period,
                                    QVector<ReporterLine> const& lines)
{
  if (!sending_ || !connected_ || call.isEmpty () || lines.isEmpty ()) return;
  Q_UNUSED (version);
  if (call.toUpper () != call_ || grid != grid_) set_identity (call, grid, version_);

  // Each pass hands over the whole period so far; only what has not gone up
  // yet goes now.
  if (period != sent_period_)
    {
      sent_period_ = period;
      sent_count_ = 0;
    }
  if (lines.size () <= sent_count_) return;

  QJsonArray items;
  for (int i = sent_count_; i < lines.size (); ++i)
    {
      auto const& l = lines[i];
      QJsonObject o;
      o["f"] = l.frequency;
      o["snr"] = l.snr;
      o["dt"] = l.dt;
      o["msg"] = l.message;
      items.append (o);
    }
  sent_count_ = lines.size ();

  QJsonObject o;
  o["t"] = "report";
  o["band"] = band;
  o["mode"] = mode;
  o["dial"] = dial;
  o["period"] = period;
  o["lines"] = items;
  queue_.append (o);
  flush_queue ();
}

void ReporterClient::flush_queue ()
{
  if (queue_.isEmpty () || send_timer_->isActive ()) return;
  auto const now = QDateTime::currentMSecsSinceEpoch ();
  auto const wait = SEND_GAP_MS - (now - last_send_ms_);
  if (wait > 0)
    {
      send_timer_->start (int (wait));
      return;
    }
  send (queue_.takeFirst ());
  last_send_ms_ = now;
  if (!queue_.isEmpty ()) send_timer_->start (SEND_GAP_MS);
}

void ReporterClient::on_message (QString const& text)
{
  if (text == "pong") return;
  auto const doc = QJsonDocument::fromJson (text.toUtf8 ());
  if (!doc.isObject ()) return;
  auto const m = doc.object ();
  auto const t = m["t"].toString ();

  if (t == "welcome" || t == "roster")
    {
      take_roster (m["stations"].toArray ());
      return;
    }

  if (t == "stream")
    {
      auto const call = m["call"].toString ();
      if (call.isEmpty () || call != following_) return;
      if (m.contains ("dial")) follow_dial_ = m["dial"].toInt ();
      if (m.contains ("band")) follow_band_ = m["band"].toString ();
      if (m.contains ("mode")) follow_mode_ = m["mode"].toString ();
      for (auto const& pv : m["periods"].toArray ())
        {
          auto const po = pv.toObject ();
          take_lines (call, po["period"].toString (), po["lines"].toArray (), true);
        }
      return;
    }

  if (t == "lines")
    {
      if (m["call"].toString () == following_)
        {
          if (m.contains ("dial")) follow_dial_ = m["dial"].toInt ();
          if (m.contains ("band")) follow_band_ = m["band"].toString ();
          if (m.contains ("mode")) follow_mode_ = m["mode"].toString ();
        }
      take_lines (m["call"].toString (), m["period"].toString (), m["lines"].toArray (), false);
      return;
    }

  if (t == "denied")
    {
      auto const call = m["call"].toString ();
      codes_.remove (call);
      if (call == following_)
        {
          following_.clear ();
          following_code_.clear ();
        }
      Q_EMIT denied (call, m["error"].toString ());
      return;
    }

  if (t == "error")
    {
      Q_EMIT trouble (tr ("MY_NET: %1").arg (m["error"].toString ()));
    }
}

void ReporterClient::take_roster (QJsonArray const& list)
{
  QVector<ReporterStation> stations;
  bool present = false;
  for (auto const& v : list)
    {
      auto const o = v.toObject ();
      ReporterStation s;
      s.call = o["call"].toString ();
      s.grid = o["grid"].toString ();
      s.band = o["band"].toString ();
      s.mode = o["mode"].toString ();
      s.dial = o["dial"].toInt ();
      s.count = o["count"].toInt ();
      s.ago = o["ago"].toDouble ();
      s.locked = o["locked"].toBool ();
      if (s.call.isEmpty ()) continue;
      if (s.call == following_)
        {
          present = true;
          // A station retunes without telling anybody but the roster.
          follow_dial_ = s.dial;
          follow_band_ = s.band;
          follow_mode_ = s.mode;
        }
      stations.append (s);
    }

  // A followed station that drops off the roster has to be said out loud; a
  // pane that simply stops is the worst thing this program can do. It stays
  // followed: if its line only dropped, it comes back with the same code.
  if (!following_.isEmpty () && present != following_present_)
    {
      following_present_ = present;
      if (present) Q_EMIT station_back (following_);
      else Q_EMIT station_gone (following_);
    }
  Q_EMIT roster_ready (stations);
}

// `whole` is a period as the server holds it, from its first line - what a
// newly followed station or a reconnect brings. Otherwise these are only the
// lines that have just arrived.
void ReporterClient::take_lines (QString const& call, QString const& period,
                                 QJsonArray const& lines, bool whole)
{
  if (call.isEmpty () || call != following_ || period.isEmpty ()) return;
  if (!last_period_.isEmpty () && period < last_period_) return;

  // The server keeps a period's lines in the order they came, so a whole
  // period that has been partly shown already has those lines counted off.
  auto const already = whole && period == last_period_ ? last_count_ : 0;
  if (period != last_period_)
    {
      last_period_ = period;
      last_count_ = 0;
    }
  int seen {0};
  QVector<ReporterLine> out;
  for (auto const& lv : lines)
    {
      if (seen++ < already) continue;
      auto const lo = lv.toObject ();
      ReporterLine line;
      line.frequency = lo["f"].toInt ();
      line.snr = lo["snr"].toInt ();
      line.dt = lo["dt"].toDouble ();
      line.message = lo["msg"].toString ();
      if (line.frequency > 0 && !line.message.isEmpty ()) out.append (line);
    }
  last_count_ = (whole ? already : last_count_) + qMax (0, seen - already);
  if (!out.isEmpty ())
    Q_EMIT lines_ready (call, period, follow_dial_, follow_band_, follow_mode_, out);
}
