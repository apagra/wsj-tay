# WSJ-TAY — what differs from WSJT-Z

A record of everything this fork does that WSJT-Z 2.0.19 does not, written for
the Help text rather than for the code. Keep it current: every change a user
could notice belongs here.

WSJ-TAY is a fork of **WSJT-Z** by SQ9FVE, which is itself a fork of **WSJT-X**
by K1JT and the WSJT development group. Everything WSJT-Z provides — Auto CQ,
Auto Call, Pounce, the filtering tabs, band hopping, QRZ lookup, the alerts —
is theirs and is documented by them. What follows is only what this fork adds
on top.

Build and deployment notes live in `BUILD-NOTES.md` instead.

---

## Two receive sources at once

Upstream decodes one sound card. This fork decodes **two simultaneously** — for
example the transceiver and an SDR — each into its own Band Activity pane.

The point is coverage: two receivers on different antennas do not hear the same
stations. In one measured cycle on 20m: 59 stations, 40 heard by both, 10 only
by the transceiver, 9 only by the second receiver.

### Setting it up

**Settings → Audio → Input 2** selects the second sound card, with its own
Mono/Left/Right. Its list begins with **None**, which is the way back to a single
receiver: the second decoder stops, its pane empties and everything belonging to
that source greys out. Upstream's list holds only real devices, which is right
for an input you cannot do without and wrong for one that is optional. Left on
None, the program behaves exactly like WSJT-Z.

Both receivers should be on the **same frequency**. The two decoders share
every setting except their audio, so decodes from the second source are
attributed to the dial frequency of the first; on a different band its spots
would be wrong.

**Input 1 is the primary.** It drives the waterfall and the timing of the
decode cycle. Put the radio you actually work with on Input 1.

### Reading the results

**Each source has its own Band Activity pane**, side by side and the same width
at the start of every session: two columns of different widths wrap their lines
differently, which is the one thing that makes them hard to compare. Dragging
them apart during a session still works, it just does not carry over. Both are complete: a station heard by both appears in
both columns, with each receiver's own SNR and DT, which is the point — that is
how you see which receiver hears it better. Drag the splitters to share out the
width. Period and band separators are drawn in both, so the columns line up.

Nothing on screen marks which source a line came from, because the pane it is
in already says so. `ALL.TXT` does mark them, `in=1` or `in=2`, since there both
sources really do share one file and the question "when did the second receiver
stop hearing anything" has to be answerable afterwards. The tag is never part
of the message, so the log and the spotting reports are unaffected.

At the end of each period a summary line appears in both panes:

```
--------------------- In 1: 42   In 2: 38 ---------------------
```

Over a few hours this is the honest answer to "is the second receiver worth it".

**Double clicking works in either pane**, and takes the callsign, the report and
the frequency from the line you actually clicked — so calling a station from the
In 2 pane sends the report *that* receiver measured.

**The station being worked is marked in both panes** — red on the callsign,
blue on its grid, if Settings → General has the DX Call and DX Grid highlights
switched on. Upstream marked the first pane only, which with two columns reads
as two different stations. The same goes for the underline on stations
reporting us to PSK Reporter.

**Erase** clears both Band Activity panes together, as do the context menu's
Clear and the periodic clear.

### Transmitting

Showing a decode and acting on one are different things. Both panes show
everything, but auto sequencing, transmission and spotting run **once per
message** however many receivers heard it, and whichever source reports it
first drives the QSO. So a reply that only the second receiver hears still
keeps the QSO moving, and nothing is ever sent twice.

### The level meters

Each source has **its own meter, standing beside the pane it belongs to**, on the
same scale so the two can be compared directly. A meter falls to zero when audio
stops arriving, so a source that has died shows as dead rather than holding its
last reading.

The reading under the bar carries its own unit (`68 dB`). Upstream kept the unit
in a second label beside the number, and the pair needed more width than a meter
standing beside a text pane can spare — the `dB` came out sliced in half.

### The panel under the In 2 pane

Everything this fork adds to the controls is gathered under the In 2 pane, in the
space where the first pane keeps its own dial and buttons. Laid out as a grid, so
the columns line up: Sync sits above Hold Rep, the readout ends the watch row.

With **Input 2** set to None the In 2 pane itself closes — an empty column of
decodes is not information — but this panel stays: the watch boxes and the
readout have nothing to do with the second source, and the controls that do are
**greyed out rather than hidden**. A control that vanishes leaves the operator
hunting for what they broke, and the room is there either way.

**Mute In 2** — stops the second source being decoded, without touching the
configuration. For comparing with and without it.

**Offset (seconds)** — shifts the second source in time. A source arriving
through a browser and a virtual audio cable is late by its buffering, which
leaves its signals outside the window the decoder searches and costs
sensitivity.

**It starts at zero and returns to zero when a QSO is logged.** A shift is either
something Sync worked out for one station or something typed for one signal; left
in place it pushes everybody else out of the decoder's window and costs decodes
without saying a word. For a standing delay — an SDR always late by the same
amount — use Sync with the callsign box empty, which measures it afresh each time
instead of trusting yesterday's number.

**call** — the callsign for Sync to steer onto. **Double clicking a station fills
it in**, since the station you have chosen to work is the one worth hearing
better, and it is emptied again when the QSO is cleared so the second source is
not left skewed towards a station you have finished with. It does nothing unless
Sync is ticked, and it can always be typed or cleared by hand.

**Sync** — sets the offset automatically:

- **call empty** — aligns the second source to the first, using the measured
  delay. This is the everyday setting: switch it on and leave it.
- **call filled** — aligns to that station instead, for digging out one signal
  whose timing sits away from nominal. While synced to it you are off everyone
  else by the same amount, so switch it off afterwards. It cannot rescue a
  station the second source never decodes at all, since it needs one decode to
  measure; for that, align to the first source instead.

Switching Sync off restores the offset it started from, and **logging the QSO
switches it off** when it was steering onto a callsign: that station is done
with, and staying on its timing leaves the second receiver off everybody else.
Sync with an empty box is the other thing entirely — it holds the second source
against the first — so that is left running.

Sync only changes how the second source is decoded. **Transmit timing always
follows UTC** and is never touched — aligning that to one station would put you
off everybody else.

**The number at the end of that row** — the measured delay of the second source
against the first, the median of the last fifty stations both heard. It appears
once at least five such stations have been seen.

**watch 1** and **watch 2** — the callsign watch boxes, which used to sit beside
*CQ only*.

**CPU** and **RAM** — what this program costs: the window and **both decoders
together**, not the machine as a whole. The decoding happens in two separate
processes, so a figure for the window alone would hide exactly what the reading
exists to answer — what the second receiver is costing. CPU is a share of every
core, so 100% would mean the whole machine busy with us. Sampled every two
seconds.

---

## Watching for callsigns

Two boxes in the panel under the In 2 pane, marked *watch 1* and *watch 2*. Enter a callsign
in either and it is marked wherever it appears, in all three panes: the first in
red, the second in blue, so the two can be told apart. Only the callsign is
marked, not the whole line, so the report and locator beside it stay readable.
Hashed callsigns, the ones printed in angle brackets, are matched too.

Both boxes start empty at every launch, deliberately. They hold whoever is being
chased right now, and a callsign left over from the last session would colour
decodes for a reason no longer on anyone's mind.

---

## Keeping the signal report

**Hold Rep**, under the Rep box, keeps the report you typed. Without it, auto
sequencing replaces it with the report measured from the next decode of the
station you are working, so a report entered by hand was silently undone a
second or two later.

**It clears itself when the QSO is logged**, and starts clear at every launch. A
held report belongs to the station it was meant for; left on, it would go
unchanged to whoever called next and everyone would be given the same number.

---

## The waterfall

Docked inside the main window rather than floating in one of its own, and with
no close or float button, so it cannot be shut by accident and leave the program
looking broken. Its title bar still drags it above or below the text panes.

It shows the first source only.

The **X** WSJT-Z put beside *Controls* is hidden. It cleared the waterfall, but
where it sat it read as a close button on a dock that deliberately cannot be
closed.

**Left click sets the receive frequency, right click the transmit frequency** —
one button, one frequency. WSJT-Z had made a plain click move both at once, with
Ctrl for receive alone, and left the right button opening a menu whose single
item moved both again; the program's own Special mouse commands help had gone on
describing the upstream behaviour all the while. Ctrl-click still moves the two
together, and Shift-click still sets transmit.

---

## The Help menu

**Help → WSJ-TAY help** is this document, in a window, and **F1** opens it.

The rest of the menu was cut back to what is true of *this* program. Upstream's
Release Notes, FT8 DXpedition Mode User Guide, Quick-Start Guide, Local User
Guide and Download Samples are gone: the first two describe a program and a mode
this is not being used for, the Quick-Start item was wired to no handler at all
and did nothing when clicked, and the Local User Guide pointed at a manual this
install does not ship.

**Help → WSJT-X User Guide (online)** replaces Online User Guide, which — and
with it F1 — had been opening a 404: WSJT-Z repointed the manual's base URL at
its groups.io page, where the file upstream builds the name of has never
existed. It is now a plain link to the published manual, which is where the
modes and every control this fork did not add are explained.

What stayed is what the licence and good manners ask for:

- **Help → Copyright notice** — the WSJT authors' own notice, which they ask to
  be displayed prominently in derivative work.
- **Help → About WSJ-TAY** — rewritten, because upstream's About called itself
  WSJT-X. It now says what this is, that it is a modified version and by whom,
  keeps the WSJT project's copyright and its memorial to G4WJS, and states the
  no-warranty and GPLv3 terms with a link to the licence and a note that anyone
  given a copy is owed the source.
- **Help → About WSJT-Z** — SQ9FVE's own, untouched.

## Smaller differences

- The title names this fork and the station it belongs to. Attribution to
  SQ9FVE and K1JT lives in Help → About WSJ-TAY, Help → About WSJT-Z and the
  `AUTHORS` file, which is where the licence wants it rather than in the title
  bar.
- **PSK Reporter is told what this actually is.** Upstream's line named the base
  project and its version, and WSJT-Z left it alone, so every station running
  either fork has been counted on the network as WSJT-X. This reports
  `WSJ-TAY beta N`. Spotting is also **on by default** for a fresh installation:
  it costs the operator nothing and the network only works because people leave
  it on. Anyone who switches it off keeps it off.
- The application is named `wsj-tay`, so it has its own settings, its own lock
  file and its own decoder shared memory, and never disturbs the stock WSJT-X,
  JTDX, MSHV or WSJT-Z installs on the same machine. No `--rig-name` needed.
- **Tools → Windows sound settings** opens the Windows Sound dialog straight on
  the Recording or the Playback tab, so a card's level can be set without
  hunting through Windows' own settings. MSHV does exactly the same.
- Every session appends to `wsj-tay-beta.log`, beside the log and the settings
  under File → Open log directory: which settings file was read, the callsign,
  all three sound cards, the rig, and any failure of the second decoder. A
  session that ends without `closed normally` died.

---

## Known limits

- Both decoders share every parameter but their samples, so the second source is
  attributed to the first's dial frequency. Keep both receivers on the same
  frequency.
- No waterfall for the second source. Its level meter and its decodes are all
  you see of it.
- A decode addressed to you that both sources hear still reaches the spotting
  report twice.
- Only one WSJT program at a time can own a sound card or OmniRig. Running this
  alongside another WSJT-X, JTDX or WSJT-Z leaves whichever started second
  without audio or rig control — it looks like a broken install but is not.

---

## Not yet carried over from the earlier WSJT-X fork

- Double clicking a combined message that carries a callsign as a hash, in
  angle brackets, on a standard FT8 sub-band.

---

## Thanks

To **SV1AER**, for the hours of testing on his own station and for the logs he
sent back. The fault that mattered most here — the second decoder dying quietly
part way through a session — was found in his `ALL.TXT`, not on this machine.
