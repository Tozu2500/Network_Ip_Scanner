@echo off
setlocal EnableExtensions

rem ============================================================
rem   Network IP Scanner - Installer
rem   Asks for permission before EVERY step. Nothing happens
rem   without an explicit "y" from you.
rem ============================================================

cd /d "%~dp0"
title Network IP Scanner - Installer

set "EXE=build\netscanner.exe"
set "SOURCES=src\main.cpp src\ui\console_ui.cpp src\ui\table_formatter.cpp src\network\adapter_info.cpp src\network\arp_scanner.cpp src\network\ping_sweep.cpp src\oui\oui_lookup.cpp"
set "SHORTCUT=%APPDATA%\Microsoft\Windows\Start Menu\Programs\Network IP Scanner.lnk"

echo.
echo   ============================================================
echo      NETWORK IP SCANNER - Installer
echo   ============================================================
echo.
echo   This installer asks before every step:
echo     Step 1: Build netscanner.exe        (writes only to .\build)
echo     Step 2: Create Start Menu shortcut  (writes one .lnk file)
echo     Step 3: Launch the app
echo.
echo   Answering "n" to a step simply skips it.
echo.

rem ---- Locate g++ (checks PATH, then common MinGW locations) ----
set "GXX=g++"
where g++ >nul 2>nul
if not errorlevel 1 goto :have_compiler
if exist "C:\msys64\mingw64\bin\g++.exe" set "GXX=C:\msys64\mingw64\bin\g++.exe"
if exist "C:\msys64\mingw64\bin\g++.exe" goto :have_compiler
if exist "C:\MinGW\bin\g++.exe" set "GXX=C:\MinGW\bin\g++.exe"
if exist "C:\MinGW\bin\g++.exe" goto :have_compiler
echo   [!] g++ not found. Install MinGW-w64 (e.g. via MSYS2) or add g++ to PATH.
pause
exit /b 1
:have_compiler

rem ---- Step 1: Build (asks first) ----
set "ANSWER=n"
set /p "ANSWER=  Step 1/3: Build netscanner.exe into .\build ? [y/N]: "
if /i not "%ANSWER%"=="y" (
    echo            Skipped build.
    goto :step_shortcut
)
echo            Building...
if not exist build mkdir build
"%GXX%" -std=c++17 -Wall -Wextra -I. %SOURCES% -o "%EXE%" -lws2_32 -liphlpapi
if errorlevel 1 (
    echo   [!] Build failed. See compiler output above.
    pause
    exit /b 1
)
echo            Build OK: %EXE%

:step_shortcut
echo.
rem ---- Step 2: Start Menu shortcut (asks first) ----
if not exist "%EXE%" (
    echo   Step 2/3: Skipped - %EXE% does not exist yet.
    goto :step_launch
)
set "ANSWER=n"
set /p "ANSWER=  Step 2/3: Create Start Menu shortcut? [y/N]: "
if /i not "%ANSWER%"=="y" (
    echo            Skipped shortcut. Nothing was written outside this folder.
    goto :step_launch
)
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$s=(New-Object -ComObject WScript.Shell).CreateShortcut('%SHORTCUT%');" ^
    "$s.TargetPath='%CD%\%EXE%';" ^
    "$s.WorkingDirectory='%CD%';" ^
    "$s.Description='Network IP Scanner';" ^
    "$s.Save()" >nul 2>nul
if exist "%SHORTCUT%" (
    echo            Shortcut created: Start Menu ^> Network IP Scanner
) else (
    echo   [!] Could not create the shortcut.
)

:step_launch
echo.
rem ---- Step 3: Launch (asks first) ----
if not exist "%EXE%" (
    echo   Step 3/3: Skipped - %EXE% does not exist yet.
    goto :done
)
set "ANSWER=n"
set /p "ANSWER=  Step 3/3: Launch Network IP Scanner now? [y/N]: "
if /i not "%ANSWER%"=="y" (
    echo            Not launched.
    goto :done
)
echo.
"%EXE%"

:done
echo.
echo   Installer finished.
endlocal
exit /b 0
