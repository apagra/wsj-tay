// The Windows recording level of a capture device, read and written from the
// program itself.
//
// It is the same level the sound settings show - not a copy of it, not a gain
// of our own. Drag the slider here and the Windows slider moves; move it there
// and ours follows at the next poll. The point is to stop the operator leaving
// the program to set a level that belongs to the card they are decoding.
//
// Kept apart from mainwindow.cpp for the same reason as resourceusage: this
// needs windows.h and COM, whose macros do not mix well with Qt in a large
// translation unit.
#ifndef WINVOLUME_H__
#define WINVOLUME_H__

#include <QString>

// Recording level of the capture endpoint whose name matches, 0 to 100.
// Returns -1 when there is no such device, when its driver offers no volume
// control, or on any platform that is not Windows - the caller greys out the
// slider rather than showing one that does nothing.
int capture_volume_percent (QString const& device_name);

// Sets that level. Returns false if it could not be set, for the same reasons.
bool set_capture_volume_percent (QString const& device_name, int percent);

// Whether this device is a virtual cable - one of the drivers that carry audio
// from another program to this one untouched. They accept a level and report it
// back, and ignore it: measured here, set to 60, read back as 60, with no change
// whatsoever to what the decoder heard, at either end of the cable. Nothing
// outside the program producing the audio can set that level, so the slider is
// greyed rather than left to be dragged for nothing. Recognised by name, which
// is all Windows offers to tell them apart.
bool device_passes_audio_through (QString const& device_name);

#endif
