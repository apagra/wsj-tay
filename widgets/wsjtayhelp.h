// Help > WSJ-TAY: what this fork adds on top of WSJT-Z.
//
// Kept in its own file rather than in mainwindow.cpp because it is a page of
// prose that will be edited far more often than the code around it.
#ifndef WSJTAYHELP_H__
#define WSJTAYHELP_H__

class QWidget;

// Shows the help window, raising the existing one if it is already open.
void show_wsj_tay_help (QWidget * parent);

#endif
