# wsj-tay — build and deployment notes (Windows / MSYS2)

Fork of **WSJT-Z 2.0.19** (base WSJT-X 3.0.0), from `github.com/sq9fve/wsjt-z`,
branch `wsj-tay`. The earlier fork of WSJT-X 2.7.0-rc4 has been deleted; its
history is archived as `old-wsjtx-fork.bundle` here (`git clone` it to read).

Goal: decode two audio sources at once — the transceiver and an SDR — each into
its own Band Activity pane. User-facing differences are in `FEATURES.md`.

## Read this before touching %LOCALAPPDATA%

**The assistant's shell runs inside the Claude desktop app's MSIX container, and
every path under `%LOCALAPPDATA%` is silently redirected** into
`%LOCALAPPDATA%\Packages\Claude_*\LocalCache\Local\`. Reads fall through to the
real file while it is untouched; the first write creates a private copy, and
from then on the two diverge invisibly.

So an assistant that "restores the settings file" changes nothing the program
will ever read, while its own checks keep showing the restore worked. Launching
the program *from that shell* makes it inherit the container and read the
private copy, so it works — and the same program launched from the desktop
reads the real file and comes up with nothing. That contradiction cost an
afternoon; the tell was the program reporting a settings file of one size at a
path where the shell saw another.

**Anything under `%LOCALAPPDATA%` — the settings file above all — is edited by
the operator, in their own terminal, or from inside the program.** The build
tree, `C:\wsj-tay` and `C:\dev` are not redirected, which is why installing a
freshly built exe there does reach the running program.

Second rule from the same afternoon: **never edit `wsj-tay.ini` with a text
tool.** Deleting a block of lines from it with `sed` left a file that Qt read as
empty, which looks exactly like lost settings. Configurations are added and
removed from the program's own Configurations menu.

## Where things live

| What | Path |
|---|---|
| Source + build | `C:\dev\wsj-tay-z` (build dir: `build\`) |
| Installed program | `C:\wsj-tay\bin\wsj-tay.exe` |
| Qt plugins | `C:\wsj-tay\plugins\` (via `bin\qt.conf`) |
| Data files | `C:\wsj-tay\share\wsjtx\` |
| Settings | `%LOCALAPPDATA%\wsj-tay\wsj-tay.ini` |
| Hamlib 4.5.5 (static) | `C:\dev\hamlib-prefix` |
| Toolchain | `C:\msys64\mingw64` |

The application name is `wsj-tay` (set in `main.cpp`), which keys the settings
directory, the lock file and the jt9 shared-memory segment. That alone keeps it
apart from the stock WSJT-X, JTDX, MSHV and WSJT-Z installs on this machine, so
**`--rig-name` is not needed** — unlike the older fork, which requires it.

## Configure and build

```
cd /c/dev/wsj-tay-z/build
PATH="/c/msys64/mingw64/bin:$PATH" \
CC=/c/msys64/mingw64/bin/gcc.exe \
CXX=/c/msys64/mingw64/bin/g++.exe \
FC=/c/msys64/mingw64/bin/gfortran.exe \
cmake -G Ninja \
  -D CMAKE_PREFIX_PATH='C:/msys64/mingw64;C:/dev/hamlib-prefix' \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_Fortran_FLAGS='-fallow-argument-mismatch' \
  -D CMAKE_CXX_FLAGS_RELEASE='-O3 -DNDEBUG -Wno-error' \
  -D WSJT_SKIP_MANPAGES=ON -D WSJT_GENERATE_DOCS=OFF ..
PATH="/c/msys64/mingw64/bin:$PATH" ninja wsjtx jt9
```

Set the compilers explicitly. An old `C:\MinGW` on the system PATH gets picked
up otherwise and fails immediately on `-fallow-argument-mismatch`.

`-Wno-error` matters: `CMakeLists.txt` puts `-Werror` in `CMAKE_CXX_FLAGS`, and
gcc 16 raises warnings the original toolchain did not. Appending through
`CMAKE_CXX_FLAGS_RELEASE` places it after `-Werror`, which is what makes it win.

Build `wsjtx jt9`, not the default target: map65 and qmap are not needed.

## Deploying: the two that cost the most time

Both were deployment, not code, and both presented as something else entirely.

1. **Qt plugins must include `audio`.** Without
   `plugins/audio/qtaudio_windows.dll`, `QAudioDeviceInfo::availableDevices()`
   returns nothing and every sound card in Settings shows *(Not found)* — which
   looks exactly like lost settings. Copy the whole plugin set, and walk the
   **plugins' own dependencies** too, not just those of the executables:
   `sqldrivers/qsqlite.dll` needs `libsqlite3-0.dll`, which no `.exe` pulls in,
   and its absence gives a fatal *Database Error: Driver not loaded*.

2. **`ALLCALL7.TXT` must sit next to `jt9.exe`.** `cwfilter.f90` opens it from
   the working directory, not from the data directory. Without it jt9 emits one
   error line per period instead of decodes, so the program looks as though it
   decodes exactly one signal per cycle. `wsjtx_dir.txt` (containing `.\`) goes
   there too, as in the reference install.

```
cd /c/dev/wsj-tay-z/build
PATH="/c/msys64/mingw64/bin:$PATH" bash -c 'for f in wsj-tay.exe jt9.exe \
  platforms/*.dll sqldrivers/*.dll audio/*.dll mediaservice/*.dll; do \
  ldd $f 2>/dev/null | grep -i "/mingw64/bin/" | awk "{print \$3}"; done \
  | sort -u | xargs -r cp -u -t .'
```

Then mirror into `C:\wsj-tay`: executables and DLLs into `bin\`, the plugin
directories into `plugins\`, `cty.dat` / `ALLCALL7.TXT` / `USState.db` /
`JPLEPH` / `eclipse.txt` into `share\wsjtx\`, and `bin\qt.conf` holding:

```
[Paths]
Plugins = ../plugins
```

Verify nothing resolves outside the install: with MSYS2 on PATH,
`ldd bin/wsj-tay.exe | grep /mingw64/bin/` must print nothing after the copy.
Before this was done, Qt was being loaded from `C:\WSJT\wsjtx\bin` — exactly
the mixing this build is supposed to avoid — and a mismatched
`libwinpthread-1.dll` gave *entry point `clock_gettime64` not found*.

## Source patches for the modern toolchain

All four were already needed by the older fork; the diagnosis transferred
whole. See the commit "Build WSJT-Z with a modern MSYS2 toolchain".

1. `lib/timer_common.inc` — threadprivate COMMON block removed; gfortran emits
   `.tls_common`, which the PE/COFF assembler cannot encode. `onlevel` grown to
   1000 and the `DATA` repeat count in `lib/timer_impl.f90` updated to match.
2. `CMake/Modules/FindFFTW3.cmake` — look for the threads libraries on Windows
   too. MSYS2 keeps them in `libfftw3f_threads`; without this jt9 fails to link
   on `fftwf_init_threads`.
3. `CMake/Modules/FindHamlib.cmake` — name Winsock when pkg-config *succeeds*.
   Our prefix ships `hamlib.pc`, so the existing WIN32 branch never ran and the
   socket symbols went unresolved.
4. `Transceiver/OmniRigTransceiver.cpp` — cast both operands to `qint64`, or
   the negative case labels are rejected as narrowing.

## Hamlib

Must be 4.x. Master is 5.0.0, which made `RIG::state` private, and
`HamlibTransceiver.cpp` reads `rig_->state.tx_vfo`.

```
git clone --depth 1 https://github.com/Hamlib/Hamlib src && cd src
git fetch --depth 1 origin tag 4.5.5 && git checkout 4.5.5
./bootstrap
../src/configure --prefix=/c/dev/hamlib-prefix --disable-shared --enable-static \
  --without-cxx-binding --disable-winradio CC=gcc CXX=g++ \
  CFLAGS='-g -O2 -fdata-sections -ffunction-sections' LDFLAGS='-Wl,--gc-sections'
make -j4 && make install
```

## Environment, not in this repo

1. **MSYS2 packages**: `qt5-base qt5-multimedia qt5-serialport qt5-tools
   qt5-translations qt5-activeqt qt5-websockets gcc-fortran fftw boost cmake
   ninja portaudio libusb` (all `mingw-w64-x86_64-*`), plus `autoconf2.73
   automake1.18 libtool make git` from the `msys` repo for Hamlib.
   `qt5-websockets` is new for this base — WSJT-Z uses it for TCI.

2. **`pacman -Syu` is mandatory.** A stale mix of `*-git` crt/headers/
   winpthreads packages against a fresh gcc produced `cc1.exe` exiting with
   `0xC0000139`, i.e. the compiler appearing to fail silently.

3. **dumpcpp**: CMake looks for `dumpcpp`, MSYS2 ships `dumpcpp-qt5.exe`.
   `cp /mingw64/bin/dumpcpp-qt5.exe /mingw64/bin/dumpcpp.exe`

4. **OmniRig is 32-bit only.** Its typelib is registered with no `win64` entry,
   so 64-bit `dumpcpp` cannot find it and CMake aborts. Fix, no admin needed:

   ```powershell
   $p = "Registry::HKEY_CURRENT_USER\SOFTWARE\Classes\TypeLib\{4FE359C5-A58F-459D-BE95-CA559FB4F270}\1.0\0\win64"
   New-Item -Path $p -Force
   Set-ItemProperty -Path $p -Name "(default)" -Value "C:\Program Files (x86)\Afreet\OmniRig\OmniRig.exe"
   ```

   Note this only fixes the *build*. At run time OmniRig still has to start; a
   *Failed to start OmniRig COM server* box means the COM server itself did not
   come up, not that rig settings were lost.

## Porting from the older fork

`patch --fuzz` is fine for the C++ where the surrounding code is unchanged, but
**never trust it for `.ui` files**. Twice it placed a block into the wrong
layout: once inside the Save-directory row instead of the soundcard grid, and
once as a bare `<item>` in a `QGridLayout`, which Qt puts at row 0 column 0, on
top of whatever is already there. Check the enclosing `<layout>` by name after
every `.ui` hunk, and run `uic` on the file before building.
