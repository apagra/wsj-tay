@echo off
setlocal enabledelayedexpansion
title WSJ-TAY installer

rem  Per-user install. No administrator rights, no registry entries, nothing
rem  added to PATH, no file associations. Uninstalling is deleting a folder.
rem  That is deliberate: the machine this was written on had three orphaned
rem  WSJT-X entries left in Installed Apps by installers that did all of those
rem  things, pointing at uninstallers that no longer existed.

set "SRC=%~dp0app"
set "DEST=%LOCALAPPDATA%\Programs\WSJ-TAY"

echo.
echo   WSJ-TAY  -  two receive sources at once
echo   ------------------------------------------------------
echo   Installing to:  %DEST%
echo.

if not exist "%SRC%\bin\wsj-tay.exe" (
  echo   ERROR: this script must sit next to the "app" folder it came with.
  echo   Extract the whole zip first, then run install.cmd from inside it.
  echo.
  pause
  exit /b 1
)

tasklist /fi "imagename eq wsj-tay.exe" 2>nul | find /i "wsj-tay.exe" >nul
if not errorlevel 1 (
  echo   WSJ-TAY is running. Close it first, then run this again.
  echo.
  pause
  exit /b 1
)

echo   Copying files...
robocopy "%SRC%" "%DEST%" /E /NFL /NDL /NJH /NJS /NP >nul
if errorlevel 8 (
  echo   ERROR: copy failed.
  echo.
  pause
  exit /b 1
)

echo   Creating the desktop shortcut...
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$w = New-Object -ComObject WScript.Shell;" ^
  "$l = $w.CreateShortcut((Join-Path ([Environment]::GetFolderPath('Desktop')) 'WSJ-TAY.lnk'));" ^
  "$l.TargetPath = '%DEST%\bin\wsj-tay.exe';" ^
  "$l.WorkingDirectory = '%DEST%\bin';" ^
  "$l.IconLocation = '%DEST%\bin\wsj-tay.exe,0';" ^
  "$l.Description = 'WSJ-TAY - decode two receive sources at once';" ^
  "$l.Save()"

echo.
echo   Done. There is a WSJ-TAY shortcut on your desktop.
echo.
echo   Nothing was written to the registry or to PATH, and no other WSJT-X,
echo   JTDX, MSHV or WSJT-Z install has been touched. WSJ-TAY keeps its own
echo   settings under:
echo       %%LOCALAPPDATA%%\wsj-tay
echo.
echo   First run: open Settings (F2) and set your callsign, locator, radio and
echo   sound cards. Settings - Audio - Input 2 is the second receiver; leave it
echo   empty and the program behaves like any other WSJT-X.
echo.
pause
endlocal
