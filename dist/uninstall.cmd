@echo off
setlocal
title WSJ-TAY uninstaller

set "DEST=%LOCALAPPDATA%\Programs\WSJ-TAY"
set "CFG=%LOCALAPPDATA%\wsj-tay"

echo.
echo   WSJ-TAY  -  uninstall
echo   ------------------------------------------------------
echo.

tasklist /fi "imagename eq wsj-tay.exe" 2>nul | find /i "wsj-tay.exe" >nul
if not errorlevel 1 (
  echo   WSJ-TAY is running. Close it first, then run this again.
  echo.
  pause
  exit /b 1
)

echo   This removes the program and the desktop shortcut:
echo       %DEST%
echo.
echo   Your settings, your log and ALL.TXT are NOT touched. They stay in:
echo       %CFG%
echo   Delete that folder by hand if you want them gone as well.
echo.
choice /c YN /m "   Remove the program"
if errorlevel 2 goto :done

if exist "%DEST%" rmdir /s /q "%DEST%"
del "%USERPROFILE%\Desktop\WSJ-TAY.lnk" 2>nul

echo.
echo   Removed.
echo.

:done
pause
endlocal
