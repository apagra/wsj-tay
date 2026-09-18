// What the machine and this program are costing, for the readout in the
// controls strip.
//
// Kept apart from mainwindow.cpp on purpose: the figures come from windows.h,
// whose macros have a long history of colliding with Qt when that header is
// pulled into a large translation unit.
#ifndef RESOURCEUSAGE_H__
#define RESOURCEUSAGE_H__

#include <QtGlobal>
#include <QVector>

// Share of the machine's processors that the given processes used over the
// interval since the previous call, as a percentage: 100 means every core was
// busy with them. Returns -1 on the first call, which has nothing to measure
// against, and on any platform that does not provide the figures.
int processes_cpu_percent (QVector<qint64> const& pids);

// Resident memory of the given processes added together, in bytes. Processes
// that are not running are skipped, so a decoder that has died simply stops
// counting.
quint64 processes_ram_bytes (QVector<qint64> const& pids);

#endif
