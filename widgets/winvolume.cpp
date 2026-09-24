#include "winvolume.h"

#ifdef Q_OS_WIN

// initguid: the property key that names a device is declared in the headers
// but defined nowhere in the mingw import libraries, so it is defined here.
#include <initguid.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

namespace
{
  // COM has to be started on this thread, and it may already be running in a
  // mode of somebody else's choosing. Either is fine; only a genuine failure
  // means we cannot go on, and only an initialisation we performed ourselves
  // gets undone.
  struct ComScope
  {
    bool ok {false};
    bool ours {false};
    ComScope ()
    {
      auto const hr = CoInitializeEx (nullptr, COINIT_APARTMENTTHREADED);
      ok = SUCCEEDED (hr) || RPC_E_CHANGED_MODE == hr;
      ours = SUCCEEDED (hr);
    }
    ~ComScope () { if (ours) CoUninitialize (); }
  };

  // Qt and Windows do not spell the same device the same way: Qt's name comes
  // through an older interface that stops at 31 characters, so "Microphone
  // (3- USB Audio CODEC)" reaches us as "Microphone (3- USB Audio CODEC )",
  // with the bracket moved. Comparing letters and digits only, over as much as
  // the shorter name offers, matches them without matching two different cards.
  QString squashed (QString const& s)
  {
    QString out;
    for (auto const c : s) { if (c.isLetterOrNumber ()) out += c.toLower (); }
    return out;
  }

  bool same_device (QString const& a, QString const& b)
  {
    auto const x = squashed (a), y = squashed (b);
    if (x.isEmpty () || y.isEmpty ()) return false;
    auto const n = qMin (24, qMin (x.size (), y.size ()));
    if (n < 6) return false;              // too little left to be sure
    return x.leftRef (n) == y.leftRef (n);
  }

  // The endpoint's volume control, or nothing. The caller owns what comes back
  // and releases it; everything borrowed on the way is released here.
  IAudioEndpointVolume * endpoint_volume (QString const& device_name, EDataFlow flow)
  {
    IMMDeviceEnumerator * enumerator {nullptr};
    if (FAILED (CoCreateInstance (__uuidof (MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  __uuidof (IMMDeviceEnumerator), (void **) &enumerator)))
      {
        return nullptr;
      }
    IMMDeviceCollection * devices {nullptr};
    IAudioEndpointVolume * found {nullptr};
    if (SUCCEEDED (enumerator->EnumAudioEndpoints (flow, DEVICE_STATE_ACTIVE, &devices)))
      {
        UINT count {0};
        devices->GetCount (&count);
        for (UINT i = 0; i < count && !found; ++i)
          {
            IMMDevice * device {nullptr};
            if (FAILED (devices->Item (i, &device))) continue;
            IPropertyStore * props {nullptr};
            if (SUCCEEDED (device->OpenPropertyStore (STGM_READ, &props)))
              {
                PROPVARIANT name;
                PropVariantInit (&name);
                if (SUCCEEDED (props->GetValue (PKEY_Device_FriendlyName, &name))
                    && name.vt == VT_LPWSTR && name.pwszVal)
                  {
                    if (same_device (QString::fromWCharArray (name.pwszVal), device_name))
                      {
                        IAudioEndpointVolume * volume {nullptr};
                        if (SUCCEEDED (device->Activate (__uuidof (IAudioEndpointVolume),
                                                         CLSCTX_ALL, nullptr, (void **) &volume)))
                          {
                            found = volume;
                          }
                      }
                  }
                PropVariantClear (&name);
                props->Release ();
              }
            device->Release ();
          }
        devices->Release ();
      }
    enumerator->Release ();
    return found;
  }

}

int capture_volume_percent (QString const& device_name)
{
  if (device_name.isEmpty ()) return -1;
  ComScope com;
  if (!com.ok) return -1;
  auto * volume = endpoint_volume (device_name, eCapture);
  if (!volume) return -1;
  float level {0.f};
  // The scalar, not the decibel figure: it is what the Windows slider itself
  // shows, so the two read alike.
  auto const hr = volume->GetMasterVolumeLevelScalar (&level);
  volume->Release ();
  if (FAILED (hr)) return -1;
  return qBound (0, int (level * 100.f + 0.5f), 100);
}

bool set_capture_volume_percent (QString const& device_name, int percent)
{
  if (device_name.isEmpty ()) return false;
  ComScope com;
  if (!com.ok) return false;
  auto * volume = endpoint_volume (device_name, eCapture);
  if (!volume) return false;
  auto const hr = volume->SetMasterVolumeLevelScalar (qBound (0, percent, 100) / 100.f, nullptr);
  volume->Release ();
  return SUCCEEDED (hr);
}

bool device_passes_audio_through (QString const& device_name)
{
  static char const * const cables[] = {
    "VB-Audio", "Virtual Cable", "Virtual Audio Cable", "VoiceMeeter",
    "CABLE Output", "CABLE Input", "Line 1 (Virtual", "Virtual Audio Device"};
  for (auto const * c : cables)
    {
      if (device_name.contains (QLatin1String {c}, Qt::CaseInsensitive)) return true;
    }
  return false;
}

#else

int capture_volume_percent (QString const&) { return -1; }
bool set_capture_volume_percent (QString const&, int) { return false; }
bool device_passes_audio_through (QString const&) { return false; }

#endif
