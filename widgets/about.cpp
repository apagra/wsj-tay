#include "about.h"

#include <QCoreApplication>
#include <QString>

#include "revision_utils.hpp"

#include "ui_about.h"

// The About box of a fork has work to do that a title bar does not: say whose
// program this started as, that it has been modified and by whom, that there
// is no warranty, under what licence it is passed on, and where the source can
// be had. The GPL asks for all of that; none of it is decoration. What is gone
// from upstream's version is only the part that was no longer true - it called
// itself WSJT-X, and showed the WSJT-X icon.
CAboutDlg::CAboutDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CAboutDlg)
{
  ui->setupUi(this);

  ui->labelTxt->setText ("<h2>" + QString {"WSJ-TAY " + wsj_tay_version ()
                                             + " " + revision ()}.simplified () + "</h2>"

    "Decodes two receive sources at the same time - a transceiver and <br />"
    "an SDR, for instance - each into its own Band Activity pane. <br />"
    "See <b>Help &gt; WSJ-TAY help</b> for what that means in use. <br /><br />"

    "&copy; 2025-2026 Alex, SV1TAY. <br />"
    "A modified version of <b>WSJT-Z</b> by Marcin, SQ9FVE, <br />"
    "which is itself a fork of <b>WSJT-X</b>. <br /><br />"

    "Tested by <b>SV1AER</b>, whose logs from his own station found <br />"
    "the fault that mattered most: the second decoder dying <br />"
    "quietly part way through a session. <br />"

    "<h3> In memory of G4WJS, Bill Somerville (1956-2021),<br />"
    "who gave so much of himself to the WSJT project. </h3>"

    "The modes, the decoders and nearly everything else here are <br />"
    "the work of the WSJT project: <br />"
    "&copy; 2001-2024 by Joe Taylor, K1JT, Bill Somerville, G4WJS, <br />"
    "Steve Franke, K9AN, Nico Palermo, IV3NWV, <br />"
    "Uwe Risse, DG2YCB, and Brian Moran, N9ADG, <br />"
    "with contributions from AC6SL, AE4JY, DF2ET, DJ0OT, G3WDG, <br />"
    "G4KLA, IW3RAB, K3WYC, KA1GT, KA6MAL, KA9Q, KB1ZMX, KD6EKQ, <br />"
    "KG4IYS, KI7MT, KK1D, ND0B, PY2SDR, VE1SKY, VK3ACF, VK4BDJ, <br />"
    "VK7MO, W3DJS, W3SZ, W4TI, W4TV, and W9MDB. <br />"
    "See <b>Help &gt; Copyright notice</b>. <br /><br />"

    "This program comes with <b>absolutely no warranty</b>. It is free <br />"
    "software under version 3 of the GNU General Public License, and <br />"
    "you are welcome to pass it on under those terms - but if you do, <br />"
    "you owe whoever you give it to the source code as well. <br />"
    "The source of this fork is available from SV1TAY; the code it is <br />"
    "built on is at github.com/sq9fve/wsjt-z and wsjt.sourceforge.io. <br /><br />"

    "<a href=\"https://www.gnu.org/licenses/gpl-3.0.txt\">"
    "<img src=\":/gpl-v3-logo.svg\" height=\"80\" /><br />"
    "https://www.gnu.org/licenses/gpl-3.0.txt</a>");
}

CAboutDlg::~CAboutDlg()
{
}
