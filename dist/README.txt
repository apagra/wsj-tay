WSJ-TAY
Decode two receive sources at once - the transceiver and a second receiver -
each into its own Band Activity pane.

by SV1TAY - sv1tay.com


INSTALLING
----------
Extract this whole zip somewhere, then run  install.cmd

It installs for the current user only, into

    %LOCALAPPDATA%\Programs\WSJ-TAY

and puts a shortcut on the desktop. It needs no administrator rights, writes
nothing to the registry, adds nothing to PATH, and touches no other program.
Uninstalling is deleting that folder - or run uninstall.cmd.


IT WILL NOT DISTURB YOUR OTHER PROGRAMS
---------------------------------------
WSJ-TAY identifies itself as "wsj-tay" rather than "WSJT-X", so it keeps its
own settings, its own lock file and its own decoder memory. An existing
WSJT-X, JTDX, MSHV or WSJT-Z install is not read, written or noticed.

Two things are still shared, because they are hardware or protocol:

  - A sound card can only be used by one program at a time.
  - So can a radio, through CAT or OmniRig.
  - Reports go out on UDP port 2237, the usual WSJT-X port, so Log4OM, JTAlert
    and GridTracker pick them up. They arrive identified as "wsj-tay", so your
    logger can tell them apart from another WSJT-X running beside it.


FIRST RUN
---------
Open Settings (F2) and fill in callsign, locator, radio and sound cards, as you
would with any WSJT-X.

Settings - Audio - Input 2 selects the second receiver. Leave it empty and the
program behaves exactly like the WSJT-Z it is built on. Set it, and you get a
second Band Activity pane, a second level meter, and a count each period of how
many stations each source decoded.

Both receivers should be on the same frequency: the two decoders share every
setting but their audio, so the second source is reported on the first one's
dial frequency.

What differs from WSJT-Z, in full, is in FEATURES.txt.


LICENCE
-------
WSJ-TAY is free software under the GNU General Public Licence, version 3.

It is a modified version of WSJT-Z by SQ9FVE, which is itself a modified
version of WSJT-X by Joe Taylor K1JT and the WSJT development group. The
licence text is in COPYING.

The licence gives you the right to the complete source code of this build.
Ask SV1TAY for it, or find the projects it derives from at

    https://github.com/sq9fve/wsjt-z          WSJT-Z
    https://sourceforge.net/projects/wsjt/    WSJT-X

This build is not endorsed by either project. Report problems with it to
SV1TAY, not to them.
