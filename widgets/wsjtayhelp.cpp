#include "wsjtayhelp.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QPointer>
#include <QString>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "revision_utils.hpp"

namespace
{
  // %1 is the fork's version. Everything WSJT-Z and WSJT-X provide is
  // documented by them; only the difference belongs here.
  char const * const help_html = R"HELP(
<h2>WSJ-TAY %1</h2>

<p>WSJ-TAY decodes <b>two receive sources at the same time</b> &mdash; a
transceiver and an SDR, for instance &mdash; each into its own Band Activity
pane.</p>

<p>It is a fork of <b>WSJT-Z</b> by SQ9FVE, which is itself a fork of
<b>WSJT-X</b> by K1JT and the WSJT Development Group. Everything those programs
do &mdash; the modes themselves, Auto CQ, Auto Call, Pounce, the filtering tabs,
band hopping, QRZ lookup, the alerts &mdash; is theirs, and is documented in
<i>Help &gt; WSJT-X User Guide</i> and <i>Help &gt; About WSJT-Z</i>. What
follows is only what this fork adds.</p>

<hr />

<h2>Two receive sources at once</h2>

<p>The point is coverage: two receivers on different antennas do not hear the
same stations. In one measured cycle on 20&nbsp;m: 59 stations, 40 heard by
both, 10 only by the transceiver, 9 only by the second receiver.</p>

<h3>Setting it up</h3>

<p><b>Settings &gt; Audio &gt; Input 2</b> selects the second sound card, with
its own Mono/Left/Right. Its list begins with <b>None</b>, which is the way back
to a single receiver: pick it and the second decoder stops, the second pane
empties and everything belonging to that source greys out. Left on None, the
program behaves exactly like WSJT-Z.</p>

<p>Both receivers should be on the <b>same frequency</b>. The two decoders share
every setting except their audio, so decodes from the second source are
attributed to the dial frequency of the first; on a different band its spots
would be wrong.</p>

<p><b>Input 1 is the primary.</b> It drives the waterfall and the timing of the
decode cycle. Put the radio you actually work with on Input 1.</p>

<h3>Reading the results</h3>

<p><b>Each source has its own Band Activity pane</b>, side by side and the same
width at the start of every session - two columns of different widths wrap their
lines differently, which is the one thing that makes them hard to compare.
Dragging them apart during a session still works. Both are complete: a station heard by both
appears in both columns, with each receiver's own SNR and DT &mdash; which is
the point, since that is how you see which receiver hears it better. Drag the
splitters to share out the width. Period and band separators are drawn in both,
so the columns line up.</p>

<p>Nothing on screen marks which source a line came from, because the pane it is
in already says so. <b>ALL.TXT</b> does mark them, <tt>in=1</tt> or
<tt>in=2</tt>, since there the two sources really do share one file and the
question "when did the second receiver stop hearing anything" has to be
answerable afterwards. The tag is never part of the message, so the log and the
spotting reports are unaffected.</p>

<p>At the end of each period a summary line appears in both panes:</p>

<pre>--------------------- In 1: 42   In 2: 38 ---------------------</pre>

<p>Over a few hours this is the honest answer to "is the second receiver worth
it".</p>

<p><b>Double clicking works in either pane</b>, and takes the callsign, the
report and the frequency from the line you actually clicked &mdash; so calling a
station from the In 2 pane sends the report <i>that</i> receiver measured. It
also fills the <b>call</b> box in the panel below, for Sync.</p>

<p><b>The station being worked is marked in both panes</b> &mdash; red on the
callsign, blue on its grid, if <i>Settings &gt; General</i> has the DX Call and
DX Grid highlights switched on. Upstream marked the first pane only, which with
two columns reads as two different stations. The same goes for the underline on
stations reporting you to PSK Reporter.</p>

<p><b>Erase</b> clears both Band Activity panes together, as do the context
menu's Clear and the periodic clear.</p>

<h3>Transmitting</h3>

<p>Showing a decode and acting on one are different things. Both panes show
everything, but auto sequencing, transmission and spotting run <b>once per
message</b> however many receivers heard it, and whichever source reports it
first drives the QSO. So a reply that only the second receiver hears still keeps
the QSO moving, and nothing is ever sent twice.</p>

<hr />

<h2>The level meters</h2>

<p>Each source has <b>its own meter, standing beside the pane it belongs to</b>,
on the same scale so the two can be compared directly. A meter falls to zero
when audio stops arriving, so a source that has died shows as dead rather than
holding its last reading.</p>

<hr />

<h2>The panel under the In 2 pane</h2>

<p>Everything this fork adds to the controls is gathered in one place, under the
In 2 pane - the space where the first pane keeps its own dial and buttons.</p>

<p>With <b>Input 2</b> set to None the In 2 pane itself closes, since an empty
column of decodes is not information, but this panel stays: the watch boxes and
the readout have nothing to do with the second source, and the controls that do
are <b>greyed out rather than hidden</b>. A control that vanishes leaves you
hunting for what you broke, and the room is there either way.</p>

<p><b>Mute In 2</b> &mdash; stops the second source being decoded, without
touching the configuration. For comparing with and without it.</p>

<p><b>Offset (seconds)</b> &mdash; shifts the second source in time. A source
arriving through a browser and a virtual audio cable is late by its buffering,
which leaves its signals outside the window the decoder searches and costs
sensitivity.</p>

<p><b>It starts at zero and returns to zero when a QSO is logged.</b> A shift is
either something Sync worked out for one station or something typed for one
signal; left in place afterwards it pushes everybody else out of the decoder's
window and costs decodes without ever saying so. To hold a standing delay - an
SDR that is always late by the same amount - use Sync with the callsign box
empty, which measures it afresh instead of trusting a number from yesterday.</p>

<p><b>call</b> &mdash; the callsign for Sync to steer onto. Double clicking a
station fills it in, since the station you have chosen to work is the one worth
hearing better, and it is emptied again when the QSO is cleared so the second
source is not left skewed towards a station you have finished with. It does
nothing unless Sync is ticked, and it can always be typed or cleared by
hand.</p>

<p><b>Sync</b> &mdash; sets the offset automatically:</p>

<ul>
<li><b>call empty</b> &mdash; aligns the second source to the first, using the
measured delay. This is the everyday setting: switch it on and leave it.</li>
<li><b>call filled</b> &mdash; aligns to that station instead, for digging out
one signal whose timing sits away from nominal. While synced to it you are off
everyone else by the same amount, so switch it off afterwards. It cannot rescue
a station the second source never decodes at all, since it needs one decode to
measure; for that, align to the first source instead.</li>
</ul>

<p>Switching Sync off restores the offset it started from, and <b>logging the
QSO switches it off for you</b> when it was steering onto a callsign: that
station is finished with, and staying aligned to its timing would leave the
second receiver off everybody else. Sync with an empty box is the other thing
entirely - it holds the second source against the first - so that is left
running.</p>

<p>Sync only changes how the second source is decoded. <b>Transmit timing always
follows UTC</b> and is never touched &mdash; aligning that to one station would
put you off everybody else.</p>

<p><b>The number at the end of that row</b> &mdash; the measured delay of the
second source against the first, the median of the last fifty stations both
heard. It appears once at least five such stations have been seen.</p>

<p><b>watch 1</b> and <b>watch 2</b> &mdash; the callsign watch boxes, described
below. They used to sit beside <i>CQ only</i>.</p>

<p><b>CPU</b> and <b>RAM</b> &mdash; what this program costs: the window and
<b>both decoders together</b>, not the machine as a whole. The decoding happens
in two separate processes, so a figure for the window alone would hide exactly
what you want to know - what the second receiver is costing you. CPU is a share
of the whole machine's processors, so 100% would mean every core busy with us.
Sampled every two seconds.</p>

<hr />

<h2>Watching for callsigns</h2>

<p>Two boxes in the panel under the In 2 pane, marked <i>watch 1</i> and
<i>watch 2</i>. Enter a callsign in either and it is marked wherever it appears, in all three
panes: the first in red, the second in blue, so the two can be told apart. Only
the callsign is marked, not the whole line, so the report and locator beside it
stay readable. Hashed callsigns, the ones printed in angle brackets, are matched
too.</p>

<p>Both boxes start empty at every launch, deliberately. They hold whoever is
being chased right now, and a callsign left over from the last session would
colour decodes for a reason no longer on anyone's mind.</p>

<hr />

<h2>Keeping the signal report</h2>

<p><b>Hold Rep</b>, under the Rep box, keeps the report you typed. Without it,
auto sequencing replaces it with the report measured from the next decode of the
station you are working, so a report entered by hand was silently undone a
second or two later.</p>

<p><b>It clears itself when the QSO is logged</b>, and starts clear at every
launch. A held report belongs to the station it was meant for; left on, it would
be sent unchanged to whoever called next, and everyone would get the same
number.</p>

<hr />

<h2>The waterfall</h2>

<p>It is docked inside the main window rather than being a separate window, and
it has no close or float buttons, so it cannot be shut by accident. Drag its
title bar to move it above or below the text panes.</p>

<p>The <b>X</b> that WSJT-Z put beside <i>Controls</i> is gone. It cleared the
waterfall, but sitting where it did it read as a close button on a dock that
deliberately cannot be closed.</p>

<p>It shows the first source only.</p>

<p><b>Left click sets the receive frequency, right click the transmit
frequency</b> &mdash; one button, one frequency. WSJT-Z had made a plain click
move both at once, with Ctrl for receive alone, and left the right button
opening a menu whose single item moved both again. <b>Ctrl-click</b> still moves
the two together, and <b>Shift-click</b> still sets transmit.</p>

<hr />

<h2>One line per message</h2>

<p><b>Decode &gt; Hide FT8 dupe messages</b> now does what its name says: one
line per message per period, the one with the best SNR.</p>

<p>Upstream's switch took effect only with the multi-threaded decoder, and on
top of that an early decode start, a VHF band or <i>Reduce false decodes</i> -
so on an ordinary HF station with the settings as they come it did nothing, and
the same station came up two and three times a period, a hertz apart, one line
carrying an AP marker and the other not.</p>

<p>Each receive source keeps its own record, so a station both receivers hear
still appears in both panes. That is the point of them.</p>

<hr />

<h2>Smaller things</h2>

<ul>
<li><b>Tools &gt; Windows sound settings</b> opens the Windows Sound dialog
straight on the Recording or the Playback tab, so the level of a card can be set
without hunting through Windows' own settings. MSHV does exactly the same.</li>
<li>The program is named <tt>wsj-tay</tt>, so it has its own settings, its own
lock file and its own decoder shared memory, and never disturbs a stock WSJT-X,
JTDX, MSHV or WSJT-Z on the same machine. No <tt>--rig-name</tt> needed.</li>
<li>Every session appends to <tt>wsj-tay-beta.log</tt>, beside the log and the
settings under <i>File &gt; Open log directory</i>: which settings file was
read, the callsign, all three sound cards, the rig, and any failure of the
second decoder. A session that ends without "closed normally" died.</li>
</ul>

<hr />

<h2>Known limits</h2>

<ul>
<li>Both decoders share every parameter but their samples, so the second source
is attributed to the first's dial frequency. Keep both receivers on the same
frequency.</li>
<li>There is no waterfall for the second source. Its level meter and its decodes
are all you see of it.</li>
<li>A decode addressed to you that both sources hear still reaches the spotting
report twice.</li>
<li>Only one WSJT program at a time can own a sound card or OmniRig. Running
this alongside another WSJT-X, JTDX or WSJT-Z leaves whichever started second
without audio or rig control &mdash; it looks like a broken install but is
not.</li>
<li>Double clicking a compound message that carries a callsign as a hash, in
angle brackets, on a standard FT8 sub-band, does not yet fill in the call.</li>
</ul>

<hr />

<hr />

<h2>Thanks</h2>

<p>To <b>SV1AER</b>, for the hours of testing on his own station and for the logs
he sent back. The fault that mattered most in this program - the second decoder
dying quietly part way through a session - was found in his ALL.TXT, not here.</p>

<p><small>WSJ-TAY is free software under the GNU General Public License,
version 3. It comes with absolutely no warranty. If you pass a copy to anyone,
you owe them the source as well. See <i>Help &gt; About WSJ-TAY</i>.</small></p>
)HELP";
}

void show_wsj_tay_help (QWidget * parent)
{
  // One window, reopened rather than stacked up. WA_DeleteOnClose together with
  // a QPointer means closing it really frees it and the pointer goes null.
  static QPointer<QDialog> dialog;

  if (!dialog)
    {
      dialog = new QDialog {parent};
      dialog->setAttribute (Qt::WA_DeleteOnClose);
      dialog->setWindowTitle (QString {"WSJ-TAY - what this adds"});

      auto * text = new QTextBrowser {dialog};
      text->setOpenExternalLinks (true);
      text->setHtml (QString {help_html}.arg (wsj_tay_version ()));

      auto * buttons = new QDialogButtonBox {QDialogButtonBox::Close, dialog};
      QObject::connect (buttons, &QDialogButtonBox::rejected, dialog, &QDialog::close);

      auto * layout = new QVBoxLayout {dialog};
      layout->addWidget (text);
      layout->addWidget (buttons);

      dialog->resize (780, 660);
    }

  dialog->show ();
  dialog->raise ();
  dialog->activateWindow ();
}
