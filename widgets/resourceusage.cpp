#include "resourceusage.h"

#if defined (Q_OS_WIN)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>

namespace
{
  quint64 as_u64 (FILETIME const& ft)
  {
    return (quint64 (ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
  }

  // Where the previous sample left off. The first call only records it.
  quint64 last_process_time {0};
  quint64 last_wall {0};
}

int processes_cpu_percent (QVector<qint64> const& pids)
{
  static int processors {0};
  if (!processors)
    {
      SYSTEM_INFO info;
      GetSystemInfo (&info);
      processors = int (info.dwNumberOfProcessors);
      if (processors < 1) processors = 1;
    }

  // Processor time these processes have consumed between them, in the same
  // 100 ns units as the clock below, so the two divide cleanly.
  quint64 used {0};
  for (auto const pid : pids)
    {
      if (pid <= 0) continue;
      auto handle = OpenProcess (PROCESS_QUERY_INFORMATION, FALSE, DWORD (pid));
      if (!handle) continue;
      FILETIME created, exited, kernel, user;
      if (GetProcessTimes (handle, &created, &exited, &kernel, &user))
        {
          used += as_u64 (kernel) + as_u64 (user);
        }
      CloseHandle (handle);
    }

  FILETIME now_ft;
  GetSystemTimeAsFileTime (&now_ft);
  auto const now = as_u64 (now_ft);

  int percent {-1};
  // A decoder that died and was restarted takes its accumulated time with it,
  // so the running total can fall. That reads as a negative interval, which is
  // meaningless rather than zero, so the sample is simply skipped.
  if (last_wall && now > last_wall && used >= last_process_time)
    {
      auto const elapsed = double (now - last_wall) * processors;
      percent = int (100.0 * double (used - last_process_time) / elapsed + 0.5);
      if (percent < 0) percent = 0;
      if (percent > 100) percent = 100;
    }

  last_process_time = used;
  last_wall = now;
  return percent;
}

quint64 processes_ram_bytes (QVector<qint64> const& pids)
{
  quint64 bytes {0};
  for (auto const pid : pids)
    {
      if (pid <= 0) continue;
      auto handle = OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, DWORD (pid));
      if (!handle) continue;
      PROCESS_MEMORY_COUNTERS counters {};
      if (GetProcessMemoryInfo (handle, &counters, sizeof counters))
        {
          bytes += counters.WorkingSetSize;
        }
      CloseHandle (handle);
    }
  return bytes;
}

#else

int processes_cpu_percent (QVector<qint64> const&) { return -1; }
quint64 processes_ram_bytes (QVector<qint64> const&) { return 0; }

#endif
