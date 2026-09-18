# WSJ-TAY

FT8 with **two receivers at once**. WSJ-TAY decodes two audio sources
simultaneously — a transceiver and an SDR, for instance — each into its own Band
Activity pane, so what one hears can be read against the other.

Two receivers on different antennas do not hear the same stations. In one
measured cycle on 20 m: 59 stations, 40 heard by both, 10 only by the
transceiver, 9 only by the second receiver.

**Download:** <https://sv1tay.com/#software>, or the
[releases](../../releases) here — Windows 10/11 x64, installs for the current
user, no administrator rights.

## What this fork adds

Two decoders with their own panes and level meters · Sync and offset for a
source arriving late through a browser and a virtual audio cable · a count per
period of what each receiver heard · callsign watch boxes · Hold Rep · one line
per message instead of duplicate decodes · in-program help (F1) · left click on
the waterfall sets receive, right click sets transmit.

`FEATURES.md` describes all of it; `dist/TI-PROSTHESAME.txt` is the same in
Greek. `BUILD-NOTES.md` has what it took to build this on Windows, traps
included.

This is a **beta**, under test at two stations.

## Where it comes from

A fork of **[WSJT-Z](https://github.com/sq9fve/wsjt-z)** by Marcin, SQ9FVE,
itself a fork of **WSJT-X** by Joe Taylor, K1JT, and the WSJT Development Group.
The modes, the decoders and nearly everything this program is are their work;
what is added here is the second receiver and what it needs to be usable.
Upstream's own README is kept as `README-WSJT-Z.md`.

Tested by **SV1AER**, whose logs from his own station found the fault that
mattered most: the second decoder dying quietly part way through a session.

## Licence

GNU General Public License v3, with absolutely no warranty — see `COPYING`.
This is the corresponding source of the published build, offered as the licence
requires.

Alex, **SV1TAY** — <https://sv1tay.com>
