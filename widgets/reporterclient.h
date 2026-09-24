// MY_NET - the station's end of the live decode sharing.
//
// One WebSocket to the server for the whole session. What the first receiver
// hears goes up it as each decoding pass finds it; the roster of who is on the
// air comes down it whenever that changes; and the lines of the station being
// followed are pushed down it as they arrive, to fill the second pane:
// borrowing somebody's ears instead of a second aerial.
//
// Following a station takes that station's code, which its operator reads out
// by hand. The code is made when this program starts and lives until it
// closes: a dropped line comes back with the same one, a restart makes a new
// one and lets everybody who was listening go.
//
// Only the first source is ever sent. The second may be a receiver somewhere
// else entirely, and passing off what it hears as this station's reception is
// the same dishonesty that keeps it out of PSK Reporter.
#ifndef REPORTERCLIENT_H__
#define REPORTERCLIENT_H__

#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>
#include <QVector>

class QTimer;
class QWebSocket;

// One decoded message as it travels: the frequency it was heard on is absolute,
// dial plus offset, because the station reading this may well be on a different
// dial and would otherwise put it in the wrong place on its own scale.
struct ReporterLine
{
  int frequency {0};            // Hz, absolute
  int snr {0};
  double dt {0.};
  QString message;
};

// A station on the roster, as the server last heard from it.
struct ReporterStation
{
  QString call;
  QString grid;
  QString band;
  QString mode;
  int dial {0};
  int count {0};                // messages in its last period
  double ago {0.};              // seconds since it last reported
  bool locked {false};          // following it takes its code
};

class ReporterClient final : public QObject
{
  Q_OBJECT

public:
  explicit ReporterClient (QObject * parent = nullptr);
  ~ReporterClient ();

  // Where the server is. Empty turns everything off.
  void set_base_url (QString const& url);

  // Who this station is, said to the server on every connection.
  void set_identity (QString const& call, QString const& grid, QString const& version);

  // Where this station is listening: band, mode and dial. Sent when it
  // changes and not otherwise, because a station with no audience sends no
  // decodes and would otherwise sit on the roster with no band beside it.
  void set_where (QString const& band, QString const& mode, int dial);

  // Whether this station's decodes are sent. Off unless the operator says so.
  void set_sending (bool);
  bool sending () const { return sending_; }

  // This station's code for the session, to be read out to whoever may listen.
  QString my_code () const { return code_; }

  // The code last used for a station this session, so choosing it again does
  // not mean typing it again. Empty if there is none.
  QString code_for (QString const& call) const;

  // Who is being listened to, empty for nobody. Not remembered between
  // sessions: tomorrow that station may not be there, and its code will not be
  // the same.
  void follow (QString const& call, QString const& code = QString {});
  QString following () const { return following_; }

  void report_period (QString const& call, QString const& grid, QString const& band,
                      QString const& mode, int dial, QString const& version,
                      QString const& period, QVector<ReporterLine> const& lines);

Q_SIGNALS:
  void roster_ready (QVector<ReporterStation> const& stations);
  void lines_ready (QString const& call, QString const& period, int dial,
                    QString const& band, QString const& mode,
                    QVector<ReporterLine> const& lines);
  // The station being followed left the roster, and later came back. It stays
  // followed in between: a dropped line comes back with the same code.
  void station_gone (QString const& call);
  void station_back (QString const& call);
  // The server would not let us follow: a wrong code, or a station that was
  // restarted and has a new one. Following has already stopped.
  void denied (QString const& call, QString const& reason);
  void trouble (QString const& what);

private:
  void open ();
  void say_hello ();
  void send_watch ();
  void send (QJsonObject const&);
  void flush_queue ();
  void on_message (QString const&);
  void take_roster (QJsonArray const&);
  void take_lines (QString const& call, QString const& period, QJsonArray const& lines,
                   bool whole);

  QWebSocket * socket_;
  QTimer * reconnect_timer_;
  QTimer * ping_timer_;
  QTimer * send_timer_;
  QString base_;
  QString call_;
  QString grid_;
  QString version_;
  QString const code_;
  QHash<QString, QString> codes_;   // call -> code, for this session only
  QString following_;
  QString following_code_;
  bool following_present_ {false};
  int follow_dial_ {0};
  QString follow_band_;
  QString follow_mode_;
  QString last_period_;         // so the same period is not shown twice
  int last_count_ {0};          // ...and lines already shown are not shown again
  QString sent_period_;         // how much of the current period has gone up
  int sent_count_ {0};
  // Nobody listening means nothing to send. The server says how many are, and
  // until one appears the period is only held here: waking the server for an
  // audience of none is somebody's money spent on nothing.
  int listeners_ {0};
  QString where_band_;
  QString where_mode_;
  int where_dial_ {0};
  QString held_band_;
  QString held_mode_;
  QString held_period_;
  int held_dial_ {0};
  QVector<ReporterLine> held_lines_;
  QList<QJsonObject> queue_;    // reports waiting their turn
  qint64 last_send_ms_ {0};
  int retry_ms_;
  bool connected_ {false};
  bool sending_ {false};
  bool said_trouble_ {false};
};

#endif
