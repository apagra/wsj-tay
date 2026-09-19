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

<p>WSJ-TAY decodes <b>two receive sources at the same time</b> &mdash; the
transceiver and a second receiver &mdash; each into its own Band Activity
pane.</p>

<p>It is a fork of <b>WSJT-Z</b> by SQ9FVE, which is itself a fork of
<b>WSJT-X</b> by K1JT and the WSJT Development Group. Everything those programs
do &mdash; the modes themselves, Auto CQ, Auto Call, Pounce, the filtering tabs,
band hopping, QRZ lookup, the alerts &mdash; is theirs, and is documented in
<i>Help &gt; WSJT-X User Guide</i> and in WSJT-Z's own documentation. What
follows is only what this fork adds.</p>

<hr />

<h2>Two receive sources at once</h2>

<p>The point is coverage: two receivers on different antennas do not hear the
same stations. In one measured cycle on 20&nbsp;m: 59 stations, 40 heard by
both, 10 only by the transceiver, 9 only by the second receiver.</p>

<h3>Setting it up</h3>

<p><b>Settings &gt; Audio &gt; Input 2</b> selects the second sound card, with
its own Mono/Left/Right. Its list begins with <b>None</b>, which is the way back
to a single receiver: the second decoder stops, its pane closes and everything
belonging to that source greys out.</p>

<hr />

<h3>One receiver, two time windows</h3>

<p>With <b>Input 2 on None</b>, setting the <b>offset</b> to anything but zero
turns the second decoder on anyway, listening to <b>the same card as Input 1</b>.
The pane then says <i>Input 2 (Input 1, shifted)</i>.</p>

<p>The point is stations whose clocks are wrong. FT8 decodes a window of time,
and a station that transmits half a second late falls outside it - the first
decoder never sees them, and nor does anyone else running things normally. The
second decoder, fed the same audio but shifted, searches where those stations
actually are. You keep the straight view in the first pane and the shifted one
beside it, instead of choosing between them.</p>

<p>Set the offset back to zero and the second decoder stops. That is deliberate:
mirroring with no offset decodes the same aerial twice for nothing, at double
the processor and memory, and the handful of decodes that differ between the two
panes differ only because the two streams began on different samples - noise
around one signal, not a second opinion.</p>

<p><b>Logging a QSO sets the offset to zero</b>, which in this mode means the
mirror stops with it. That is the same rule as everywhere else - a shift belongs
to the moment it was set for, not to the rest of the day - so after working one
of these off-timing stations, type the offset again to carry on hunting. The
first pane never stops, whatever the offset does.</p>

<p>The shift belongs to the second source and always will. Input 1 sets the
timing of everything - the waterfall, the decode cycle, the DT column - so
shifting it would shift the lot, and you would dig out the odd station by losing
everyone who has their clock right.</p>

<p>Choosing the first card explicitly in the Input 2 list does the same thing,
except it stays on whatever the offset says. The program says so once, when you
pick it.</p>

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

<p>The <b>offset</b> is the exception: it stays usable, because it is the switch
that starts the second decoder on the first card - see <i>One receiver, two time
windows</i> above.</p>

<p><b>Mute In 2</b> &mdash; stops the second source being decoded, without
touching the configuration. For comparing with and without it.</p>

<p><b>Offset (seconds)</b> &mdash; shifts the second source in time. A source
arriving through a browser and a virtual audio cable is late by its buffering,
which leaves its signals outside the window the decoder searches and costs
sensitivity.</p>

<p><b>It starts at zero and returns to zero when a QSO is logged.</b> A shift is
either something Sync worked out for one station or something typed for one
signal; left in place afterwards it pushes everybody else out of the decoder's
window and costs decodes without ever saying so. To hold a standing delay - a
second receiver that is always late by the same amount - use Sync with the callsign box
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

<p>It shows the first source only &mdash; but not quite. <b>Green marks</b> show
where the <b>second</b> receiver heard a station that <b>the first one did
not</b>: on the frequency scale for the period just decoded, and painted into
the waterfall itself, where they travel down with the picture and leave a short
green column under the period they belong to.</p>

<p>They are there because of a trap: you choose where to transmit by eye, from
this waterfall, and a station only the second receiver hears leaves no trace on
it. Answer one of those and your signal can land straight on top of somebody you
cannot see. The green marks are that somebody.</p>

<p>The <b>In 2 graph</b> box beside <i>Controls</i>, at the top of the waterfall,
turns the marks off and on; the setting is remembered.</p>

<p>Only the blind spot is marked, never the whole of the second receiver's
traffic. Marking everything filled the scale with ticks and said no more than
"there is FT8 here", which the waterfall says already.</p>

<p>Decodes only. Energy that never turns into a decode - a weak signal, a
carrier, plain QRM - leaves no mark, which is the honest limit of marking the
first waterfall instead of building a second one. The marks also trail the
signal they refer to by about one period, since the comparison can only be made
once both decoders have finished with it.</p>

<p><b>Left click sets the receive frequency, right click the transmit
frequency</b> &mdash; one button, one frequency. WSJT-Z had made a plain click
move both at once, with Ctrl for receive alone, and left the right button
opening a menu whose single item moved both again. <b>Ctrl-click</b> still moves
the two together, and <b>Shift-click</b> still sets transmit.</p>

<hr />

<h2>When Windows moves the sound cards about</h2>

<p>Change the default playback device in Windows - speakers to a virtual cable,
say - and the stream this program opened at startup can be left pointing at the
old card. The sign is transmit audio going to the speakers while the radio gets
nothing, and until now the only cure was to restart.</p>

<p>The list of sound devices is checked every few seconds. When it changes, the
program says so and offers to <b>re-open the audio</b> on the cards your settings
name, looked up afresh at that moment - which is the point, since it is the
lookup made at startup that went stale.</p>

<p>Nothing is reopened behind your back, and never while you are transmitting or
tuning: if the cards change mid-transmission, the question waits for the next
quiet moment. Only the side that changed is touched, so an output that moved does
not cost you a period of decodes on the inputs.</p>

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
<li>Only one program at a time can hold the rig: OmniRig, or a serial port for
CAT. Running this alongside another WSJT-X, JTDX or WSJT-Z leaves whichever
started second without rig control - it looks like a broken install but is not.
Sound cards are different: Windows shares a capture device between programs, and
this one relies on that to feed both decoders from one card.</li>
<li>Double clicking a compound message that carries a callsign as a hash, in
angle brackets, on a standard FT8 sub-band, does not yet fill in the call.</li>
</ul>

<hr />

<h2>Thanks</h2>

<p>To <b>SV1AER</b>, for the hours of testing on his own station and for the logs
he sent back. The fault that mattered most in this program - the second decoder
dying quietly part way through a session - was found in his ALL.TXT, not here.</p>

<p><small>WSJ-TAY is free software under the GNU General Public License,
version 3. It comes with absolutely no warranty. If you pass a copy to anyone,
you owe them the source as well. See <i>Help &gt; About WSJ-TAY</i>.</small></p>

<hr />
<hr />

<h1>&Sigma;&tau;&alpha; &epsilon;&lambda;&lambda;&eta;&nu;&iota;&kappa;ά</h1>

<h2>WSJ-TAY %1</h2>

<p>Το WSJ-TAY αποκωδικοποιεί <b>δύο πηγές ήχου ταυτόχρονα</b> &mdash; τον
πομποδέκτη και έναν δεύτερο δέκτη &mdash; η καθεμία στο δικό της παράθυρο.</p>

<p>Είναι παραλλαγή του <b>WSJT-Z</b> του SQ9FVE, που με τη σειρά του είναι
παραλλαγή του <b>WSJT-X</b> του K1JT και της ομάδας WSJT. Ό,τι κάνουν εκείνα τα
προγράμματα &mdash; τα modes, Auto CQ, Auto Call, Pounce, τα φίλτρα, band
hopping, QRZ lookup, οι ειδοποιήσεις &mdash; είναι δικό τους. Παρακάτω είναι
μόνο όσα προσθέτει αυτή η παραλλαγή.</p>

<hr />

<h2>Δύο δέκτες ταυτόχρονα</h2>

<p>Το νόημα είναι η κάλυψη: δύο δέκτες σε διαφορετικές κεραίες δεν ακούνε τους
ίδιους σταθμούς. Σε μια μετρημένη περίοδο στα 20&nbsp;m: 59 σταθμοί, 40 τους
άκουσαν και οι δύο, 10 μόνο ο πομποδέκτης, 9 μόνο ο δεύτερος δέκτης.</p>

<h3>Ρύθμιση</h3>

<p><b>Settings &gt; Audio &gt; Input 2</b> επιλέγει τη δεύτερη κάρτα, με δικό της
Mono/Left/Right. Η λίστα ξεκινάει με <b>None</b>, που είναι ο δρόμος πίσω σε
έναν δέκτη: ο δεύτερος αποκωδικοποιητής σταματά, το παράθυρό του κλείνει και όσα
αφορούν εκείνη την πηγή γκριζάρουν.</p>

<h3>Ένας δέκτης, δύο χρονικά παράθυρα</h3>

<p>Με το <b>Input 2 στο None</b>, αν βάλεις <b>offset</b> διάφορο του μηδενός, ο
δεύτερος αποκωδικοποιητής ανάβει έτσι κι αλλιώς &mdash; στην <b>ίδια κάρτα με το
Input 1</b>. Ο τίτλος γράφει τότε <i>Input 2 (Input 1, shifted)</i>.</p>

<p>Είναι για τους σταθμούς με χαλασμένο ρολόι. Το FT8 αποκωδικοποιεί ένα χρονικό
παράθυρο, και όποιος εκπέμπει μισό δευτερόλεπτο αργά πέφτει έξω από αυτό &mdash;
δεν τον βλέπει ο πρώτος αποκωδικοποιητής, δεν τον βλέπει και κανένας άλλος που
δουλεύει κανονικά. Ο δεύτερος, με τον ίδιο ήχο μετατοπισμένο, ψάχνει εκεί που
πραγματικά βρίσκονται. Κρατάς την κανονική εικόνα στο πρώτο παράθυρο και τη
μετατοπισμένη δίπλα της, αντί να διαλέγεις.</p>

<p>Γυρνάς το offset στο μηδέν και ο δεύτερος σβήνει. Καθρέφτης χωρίς μετατόπιση
σημαίνει να αποκωδικοποιείς δύο φορές την ίδια κεραία για το τίποτα, με διπλή
CPU και μνήμη.</p>

<p><b>Η καταγραφή του QSO μηδενίζει το offset</b>, άρα σβήνει και ο καθρέφτης: η
μετατόπιση ανήκει στη στιγμή για την οποία μπήκε. Ξαναβάζεις το offset και
συνεχίζεις. Το πρώτο παράθυρο δεν σταματάει ποτέ.</p>

<p>Η μετατόπιση ανήκει πάντα στη <b>δεύτερη</b> πηγή. Το Input 1 ορίζει τον
χρονισμό των πάντων &mdash; waterfall, κύκλο αποκωδικοποίησης, στήλη DT &mdash;
οπότε μετατοπίζοντάς το θα μετατόπιζες τα πάντα, και θα ξέθαβες έναν σταθμό
χάνοντας όλους όσοι έχουν σωστό ρολόι.</p>

<h3>Ίδια συχνότητα</h3>

<p>Οι δύο δέκτες πρέπει να είναι στην <b>ίδια συχνότητα</b>. Οι δύο
αποκωδικοποιητές μοιράζονται κάθε ρύθμιση εκτός από τον ήχο, οπότε ό,τι ακούει ο
δεύτερος καταγράφεται στη συχνότητα του πρώτου.</p>

<p><b>Το Input 1 είναι το κύριο.</b> Αυτό οδηγεί το waterfall και τον χρονισμό
των κύκλων. Βάλε εκεί το ραδιόφωνο που πραγματικά δουλεύεις.</p>

<h3>Τι βλέπεις</h3>

<p>Κάθε πηγή έχει <b>δικό της παράθυρο Band Activity</b>, δίπλα-δίπλα και με ίδιο
πλάτος σε κάθε άνοιγμα. Και τα δύο είναι πλήρη: σταθμός που ακούστηκε και από
τους δύο εμφανίζεται και στα δύο, με το δικό του SNR και DT στο καθένα &mdash;
αυτό ακριβώς είναι το ζητούμενο.</p>

<p>Στην οθόνη δεν μπαίνει σήμανση για το ποια πηγή έδωσε τη γραμμή: το ίδιο το
παράθυρο το λέει. Στο <b>ALL.TXT</b> μπαίνει, <tt>in=1</tt> ή <tt>in=2</tt>,
γιατί εκεί οι δύο πηγές γράφουν στο ίδιο αρχείο και πρέπει να μπορείς να βρεις
εκ των υστέρων πότε σταμάτησε να ακούει ο δεύτερος δέκτης.</p>

<p>Στο τέλος κάθε περιόδου μπαίνει σύνοψη και στα δύο παράθυρα:</p>

<pre>--------------------- In 1: 42   In 2: 38 ---------------------</pre>

<p>Μετά από μερικές ώρες, αυτό είναι η ειλικρινής απάντηση στο &laquo;αξίζει ο
δεύτερος δέκτης;&raquo;.</p>

<p><b>Το διπλό κλικ δουλεύει και στα δύο παράθυρα</b>, και παίρνει callsign,
report και συχνότητα από τη γραμμή που πάτησες &mdash; άρα καλώντας από το
παράθυρο In 2 στέλνεις το σήμα που μέτρησε <i>εκείνος</i> ο δέκτης. Συμπληρώνει
και το κουτί <b>call</b> στο ταμπλό, για το Sync.</p>

<p><b>Ο σταθμός που δουλεύεις μαρκάρεται και στα δύο παράθυρα</b> &mdash; κόκκινο
το callsign, μπλε το grid, αν είναι αναμμένα τα Highlight DX Call/Grid στο
Settings &gt; General.</p>

<p><b>Το Erase</b> καθαρίζει και τα δύο παράθυρα μαζί.</p>

<h3>Εκπομπή</h3>

<p>Άλλο το να εμφανίζεις ένα decode, άλλο το να ενεργήσεις πάνω του. Και τα δύο
παράθυρα δείχνουν τα πάντα, αλλά η αυτόματη ακολουθία, η εκπομπή και τα spotting
γίνονται <b>μία φορά ανά μήνυμα</b>, όσοι δέκτες κι αν το άκουσαν. Έτσι μια
απάντηση που την ακούει μόνο ο δεύτερος δέκτης συνεχίζει κανονικά το QSO, και
τίποτα δεν στέλνεται δύο φορές.</p>

<hr />

<h2>Οι μπάρες στάθμης</h2>

<p>Κάθε πηγή έχει <b>δική της μπάρα, δίπλα στο παράθυρο που της ανήκει</b>, στην
ίδια κλίμακα ώστε να συγκρίνονται. Πέφτει στο μηδέν όταν σταματήσει να έρχεται
ήχος, οπότε μια πηγή που πέθανε φαίνεται αμέσως.</p>

<hr />

<h2>Το ταμπλό κάτω από το παράθυρο In 2</h2>

<p><b>Mute In 2</b> &mdash; σταματάει την αποκωδικοποίηση της δεύτερης πηγής,
χωρίς να πειράξει τις ρυθμίσεις.</p>

<p><b>Offset (δευτερόλεπτα)</b> &mdash; μετατοπίζει χρονικά τη δεύτερη πηγή. Ήχος
που έρχεται μέσω browser και εικονικού καλωδίου καθυστερεί από το buffering, και
τα σήματά του πέφτουν έξω από το παράθυρο που ψάχνει ο αποκωδικοποιητής.
<b>Ξεκινάει στο μηδέν και γυρίζει στο μηδέν με την καταγραφή του QSO.</b></p>

<p><b>call</b> &mdash; το callsign στο οποίο θα στοχεύσει το Sync. Το διπλό κλικ
το συμπληρώνει μόνο του, και αδειάζει όταν καθαρίσει η επαφή.</p>

<p><b>Sync</b> &mdash; ρυθμίζει το offset αυτόματα: με <b>άδειο call</b>
ευθυγραμμίζει τη δεύτερη πηγή με την πρώτη (η καθημερινή χρήση), με
<b>συμπληρωμένο call</b> ευθυγραμμίζει με εκείνον τον σταθμό. Σβήνοντάς το
επιστρέφει το offset που είχες πριν, και η καταγραφή του QSO το σβήνει όταν ήταν
κουμπωμένο πάνω σε callsign. Το Sync αλλάζει <b>μόνο</b> πώς αποκωδικοποιείται η
δεύτερη πηγή· ο χρονισμός εκπομπής ακολουθεί πάντα το UTC.</p>

<p><b>Ο αριθμός στο τέλος της σειράς</b> &mdash; η μετρημένη καθυστέρηση της
δεύτερης πηγής ως προς την πρώτη, η διάμεσος των τελευταίων 50 σταθμών που
άκουσαν και οι δύο. Εμφανίζεται μόλις μαζευτούν τουλάχιστον 5.</p>

<p><b>watch 1 / watch 2</b> &mdash; βάζεις ένα callsign και μαρκάρεται όπου
εμφανιστεί, και στα τρία παράθυρα: το πρώτο κόκκινο, το δεύτερο μπλε. Μαρκάρεται
μόνο το callsign, όχι όλη η γραμμή. Τα κουτιά ξεκινούν <b>άδεια</b> σε κάθε
άνοιγμα, σκόπιμα.</p>

<p><b>CPU / RAM</b> &mdash; τι κοστίζει <b>το πρόγραμμα</b>: το παράθυρο και οι
δύο αποκωδικοποιητές μαζί, όχι όλο το μηχάνημα. Δείγμα κάθε δύο δευτερόλεπτα.</p>

<hr />

<h2>Παρακολούθηση callsign</h2>

<p>Δύο κουτάκια στο ταμπλό κάτω από το παράθυρο In 2, με την ένδειξη
<i>watch 1</i> και <i>watch 2</i>. Γράφεις ένα callsign σε όποιο θέλεις και
μαρκάρεται παντού όπου εμφανίζεται, και στα τρία παράθυρα: το πρώτο κόκκινο,
το δεύτερο μπλε, για να ξεχωρίζουν. Μαρκάρεται μόνο το callsign και όχι όλη η
γραμμή, ώστε να διαβάζονται το report και το locator δίπλα του. Πιάνονται και
τα hashed callsign, αυτά που τυπώνονται σε γωνιακές αγκύλες.</p>

<p>Και τα δύο κουτάκια ξεκινάνε άδεια σε κάθε εκκίνηση, επίτηδες. Κρατάνε
όποιον κυνηγάς <i>τώρα</i>&mdash; ένα callsign ξεχασμένο από την προηγούμενη
φορά θα χρωμάτιζε decode για λόγο που δεν θυμάται πια κανείς.</p>

<hr />

<h2>Hold Rep</h2>

<p>Κάτω από το κουτί Rep. Κρατάει το report που πληκτρολόγησες &mdash; χωρίς
αυτό, η αυτόματη ακολουθία το αντικαθιστά με αυτό που μέτρησε στο επόμενο decode.
<b>Σβήνει μόνο του όταν καταγραφεί το QSO</b>, και ξεκινάει σβηστό σε κάθε
άνοιγμα: το κρατημένο report ανήκει στον σταθμό για τον οποίο μπήκε.</p>

<hr />

<h2>Ο καταρράκτης</h2>

<p>Είναι καρφωμένος μέσα στο κεντρικό παράθυρο και δεν έχει κουμπιά κλεισίματος,
ώστε να μην κλείνει κατά λάθος. Δείχνει μόνο την πρώτη πηγή. Το <b>X</b> που
είχε βάλει δίπλα στο Controls το WSJT-Z είναι κρυμμένο &mdash; καθάριζε τον
καταρράκτη, αλλά διαβαζόταν σαν κουμπί κλεισίματος.</p>

<p><b>Πράσινα σημάδια</b> δείχνουν πού άκουσε ο <b>δεύτερος</b> δέκτης σταθμό που
<b>ΔΕΝ άκουσε ο πρώτος</b>: στην κλίμακα συχνοτήτων για την περίοδο που μόλις
αποκωδικοποιήθηκε, και ζωγραφισμένα μέσα στον ίδιο τον καταρράκτη, όπου
κατεβαίνουν μαζί με την εικόνα και αφήνουν μια κοντή πράσινη στήλη κάτω από την
περίοδο στην οποία ανήκουν.</p>

<p>Το κουτάκι <b>In 2 graph</b> δίπλα στο <i>Controls</i>, πάνω από τον
καταρράκτη, ανάβει και σβήνει τα σημάδια &mdash; και η επιλογή θυμάται.</p>

<p>Μαρκάρεται μόνο το τυφλό σημείο, ποτέ όλη η κίνηση του δεύτερου δέκτη. Όταν
τα μαρκάραμε όλα, η κλίμακα γέμισε γραμμές που δεν έλεγαν τίποτα περισσότερο από
&laquo;εδώ υπάρχει FT8&raquo; &mdash; που το λέει ήδη ο καταρράκτης. Τα σημάδια
επίσης καθυστερούν κατά μία περίοδο, αφού η σύγκριση γίνεται μόνο όταν
τελειώσουν και οι δύο αποκωδικοποιητές.</p>

<p>Υπάρχουν για μια παγίδα: διαλέγεις πού θα εκπέμψεις με το μάτι, από αυτόν τον
καταρράκτη &mdash; και ένας σταθμός που τον ακούει μόνο ο δεύτερος δέκτης δεν
αφήνει κανένα ίχνος εδώ. Απαντάς σε έναν τέτοιο και το σήμα σου μπορεί να πέσει
ακριβώς πάνω σε κάποιον που δεν βλέπεις. Τα σημάδια είναι αυτός ο κάποιος.</p>

<p>Μόνο decode. Ενέργεια που δεν βγάζει decode &mdash; αδύναμο σήμα, φέρον,
σκέτο QRM &mdash; δεν αφήνει σημάδι. Αυτό είναι το τίμημα του να μαρκάρουμε τον
πρώτο καταρράκτη αντί να φτιάξουμε δεύτερο.</p>

<p><b>Αριστερό κλικ βάζει τη λήψη, δεξί κλικ την εκπομπή</b> &mdash; ένα κουμπί,
μία συχνότητα. Το WSJT-Z είχε κάνει το σκέτο κλικ να κουνάει και τα δύο.
<b>Ctrl+κλικ</b> τα κουνάει και τα δύο μαζί, <b>Shift+κλικ</b> βάζει την
εκπομπή.</p>

<hr />

<h2>Όταν τα Windows κουνάνε τις κάρτες ήχου</h2>

<p>Αλλάζεις την προεπιλεγμένη συσκευή αναπαραγωγής στα Windows &mdash; από τα
ηχεία σε εικονικό καλώδιο, ας πούμε &mdash; και η ροή που άνοιξε το πρόγραμμα
στην εκκίνηση μπορεί να μείνει κολλημένη στην παλιά κάρτα. Το σημάδι είναι ότι
η εκπομπή φεύγει στα ηχεία και το ραδιόφωνο δεν παίρνει τίποτα· μέχρι τώρα η
μόνη λύση ήταν επανεκκίνηση.</p>

<p>Η λίστα των συσκευών ελέγχεται κάθε λίγα δευτερόλεπτα. Όταν αλλάξει, το
πρόγραμμα σου το λέει και προσφέρεται να <b>ξανανοίξει τον ήχο</b> στις κάρτες
που λένε οι ρυθμίσεις σου, ψάχνοντάς τες εκείνη τη στιγμή &mdash; εκεί ήταν το
πρόβλημα, στην αναζήτηση της εκκίνησης που είχε μπαγιατέψει.</p>

<p>Τίποτα δεν ξανανοίγει στα κρυφά, και ποτέ όσο εκπέμπεις ή κάνεις tune: αν η
αλλαγή συμβεί στη μέση εκπομπής, η ερώτηση περιμένει την πρώτη ήσυχη στιγμή.
Πειράζεται μόνο η πλευρά που άλλαξε, οπότε μια έξοδος που κουνήθηκε δεν σου
κοστίζει μια περίοδο από decode στις εισόδους.</p>

<hr />

<h2>Μία γραμμή ανά μήνυμα</h2>

<p><b>Decode &gt; Hide FT8 dupe messages</b> κάνει πια αυτό που λέει το όνομά
του: μία γραμμή ανά μήνυμα ανά περίοδο, αυτή με το καλύτερο SNR. Στο κανονικό
πρόγραμμα ο διακόπτης έπιανε μόνο με τον multi-threaded αποκωδικοποιητή και από
πάνω ήθελε κι άλλες προϋποθέσεις &mdash; δηλαδή σε συνηθισμένο σταθμό HF δεν
έκανε τίποτα. Κάθε πηγή κρατάει <b>δικό της</b> ιστορικό, οπότε σταθμός που τον
ακούνε και οι δύο δέκτες εμφανίζεται κανονικά και στα δύο παράθυρα.</p>

<hr />

<h2>Μικρότερα</h2>

<ul>
<li><b>Tools &gt; Windows sound settings</b>: ανοίγει το παράθυρο Ήχου των
Windows κατευθείαν στην καρτέλα Εγγραφή ή Αναπαραγωγή.</li>
<li>Το πρόγραμμα ονομάζεται <tt>wsj-tay</tt>, οπότε έχει δικές του ρυθμίσεις και
δεν ενοχλεί κανένα WSJT-X, JTDX, MSHV ή WSJT-Z στο ίδιο μηχάνημα.</li>
<li>Κάθε συνεδρία γράφει στο <tt>wsj-tay-beta.log</tt>, δίπλα στις ρυθμίσεις
(<i>File &gt; Open log directory</i>): ποιο αρχείο ρυθμίσεων διαβάστηκε,
callsign, κάρτες ήχου, ραδιόφωνο, και κάθε αποτυχία του δεύτερου
αποκωδικοποιητή. Συνεδρία που τελειώνει χωρίς &laquo;closed normally&raquo;
πέθανε.</li>
</ul>

<hr />

<h2>Γνωστοί περιορισμοί</h2>

<ul>
<li>Οι δύο αποκωδικοποιητές μοιράζονται κάθε παράμετρο εκτός από τα δείγματα,
οπότε η δεύτερη πηγή αποδίδεται στη συχνότητα της πρώτης.</li>
<li>Δεν υπάρχει waterfall για τη δεύτερη πηγή.</li>
<li>Decode που απευθύνεται σε εσένα και το ακούνε και οι δύο πηγές φτάνει δύο
φορές στα δίκτυα spotting.</li>
<li>Μόνο ένα πρόγραμμα τη φορά κρατάει το ραδιόφωνο (OmniRig ή σειριακή CAT). Οι
κάρτες ήχου είναι άλλο θέμα: τα Windows τις μοιράζουν, και αυτό το πρόγραμμα
βασίζεται σε αυτό για να ταΐζει δύο αποκωδικοποιητές από μία κάρτα.</li>
<li>Το διπλό κλικ σε σύνθετο μήνυμα με callsign σε αγκύλες, στις κανονικές
υποζώνες FT8, δεν συμπληρώνει ακόμα το call.</li>
</ul>

<hr />

<h2>Ευχαριστίες</h2>

<p>Στον <b>SV1AER</b>, για τις ώρες δοκιμών στον δικό του σταθμό και για τα log
που έστειλε πίσω. Το σοβαρότερο σφάλμα αυτού του προγράμματος &mdash; ο δεύτερος
αποκωδικοποιητής που πέθαινε αθόρυβα στη μέση της συνεδρίας &mdash; βρέθηκε στο
δικό του ALL.TXT, όχι εδώ.</p>

<p><small>Ελεύθερο λογισμικό υπό την GNU General Public License v3, χωρίς καμία
εγγύηση. Αν το δώσεις σε κάποιον, του χρωστάς και τον πηγαίο κώδικα. Δες
<i>Help &gt; About WSJ-TAY</i>.</small></p>
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
