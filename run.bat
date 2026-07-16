@echo off
setlocal EnableExtensions

rem ============================================================
rem   Network IP Scanner - Build & Run
rem   Only writes to .\build inside this folder, never elsewhere.
rem ============================================================

cd /d "%~dp0"

set "EXE=build\netscanner.exe"
set "SOURCES=src\main.cpp src\ui\console_ui.cpp src\ui\table_formatter.cpp src\network\adapter_info.cpp src\network\arp_scanner.cpp src\network\ping_sweep.cpp src\oui\oui_lookup.cpp"

rem ---- Locate g++ (checks PATH, then common MinGW locations) ----
set "GXX=g++"
where g++ >nul 2>nul
if not errorlevel 1 goto :have_compiler
if exist "C:\msys64\mingw64\bin\g++.exe" set "GXX=C:\msys64\mingw64\bin\g++.exe"
if exist "C:\msys64\mingw64\bin\g++.exe" goto :have_compiler
if exist "C:\MinGW\bin\g++.exe" set "GXX=C:\MinGW\bin\g++.exe"
if exist "C:\MinGW\bin\g++.exe" goto :have_compiler
echo [!] g++ not found. Install MinGW-w64 (e.g. via MSYS2) or add g++ to PATH.
pause
exit /b 1
:have_compiler

rem ---- Decide whether to build ----
if not exist "%EXE%" goto :ask_build

set "ANSWER=n"
set /p "ANSWER=Found %EXE%. Rebuild it before running? [y/N]: "
if /i "%ANSWER%"=="y" goto :build
goto :run

:ask_build
set "ANSWER=y"
set /p "ANSWER=%EXE% not found. Build it now? (writes only to .\build) [Y/n]: "
if /i "%ANSWER%"=="n" (
    echo Nothing was built. Bye!
    exit /b 0
)

:build
echo.
echo Building netscanner.exe ...
if not exist build mkdir build
"%GXX%" -std=c++17 -Wall -Wextra -I. %SOURCES% -o "%EXE%" -lws2_32 -liphlpapi
if errorlevel 1 (
    echo.
    echo [!] Build failed. See compiler output above.
    pause
    exit /b 1
)
echo Build OK.

:run
echo.
"%EXE%"
exit /b %errorlevel%
