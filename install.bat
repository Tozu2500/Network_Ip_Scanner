@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem ============================================================
rem   Network IP Scanner - Windows Installer
rem ============================================================

title Network IP Scanner - Installer
color 0B

echo.
echo   ============================================================
echo      NETWORK IP SCANNER - Installer
echo   ============================================================
echo.

rem --- Move to the script's own directory ---
cd /d "%~dp0"

rem --- [1/4] Locate a C++ toolchain (mingw32-make) ------------
echo   [1/4] Checking for build toolchain...
where mingw32-make >nul 2>nul
if errorlevel 1 (
    if exist "C:\msys64\mingw64\bin\mingw32-make.exe" (
        set "PATH=C:\msys64\mingw64\bin;%PATH%"
        echo         Found MinGW at C:\msys64\mingw64\bin
    ) else if exist "C:\MinGW\bin\mingw32-make.exe" (
        set "PATH=C:\MinGW\bin;%PATH%"
        echo         Found MinGW at C:\MinGW\bin
    ) else (
        echo.
        echo   [X] No C++ toolchain found ^(mingw32-make^).
        echo       Install MinGW-w64 ^(e.g. via MSYS2^) or add g++ to PATH.
        echo.
        pause
        exit /b 1
    )
) else (
    echo         mingw32-make found on PATH.
)

rem --- [2/4] Build the application ----------------------------
echo.
echo   [2/4] Building netscanner.exe...
mingw32-make >build.log 2>&1
if errorlevel 1 (
    echo.
    echo   [X] Build failed. See build.log for details:
    echo   ------------------------------------------------------------
    type build.log
    echo   ------------------------------------------------------------
    pause
    exit /b 1
)
echo         Build succeeded.
del build.log >nul 2>nul

rem --- [3/4] Verify the produced executable -------------------
echo.
echo   [3/4] Verifying executable...
if not exist "build\netscanner.exe" (
    echo   [X] build\netscanner.exe was not produced.
    pause
    exit /b 1
)
echo         Found build\netscanner.exe

rem --- [4/4] Create a Start Menu shortcut ---------------------
echo.
echo   [4/4] Creating Start Menu shortcut...
set "SHORTCUT=%APPDATA%\Microsoft\Windows\Start Menu\Programs\Network IP Scanner.lnk"
set "TARGET=%CD%\build\netscanner.exe"
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$s=(New-Object -ComObject WScript.Shell).CreateShortcut('%SHORTCUT%');" ^
    "$s.TargetPath='%TARGET%';" ^
    "$s.WorkingDirectory='%CD%';" ^
    "$s.Description='Network IP Scanner';" ^
    "$s.Save()" >nul 2>nul
if exist "%SHORTCUT%" (
    echo         Shortcut created: Start Menu ^> Network IP Scanner
) else (
    echo         [!] Could not create Start Menu shortcut ^(skipped^).
)

rem --- Done ---------------------------------------------------
echo.
echo   ============================================================
echo      Installation complete!
echo   ============================================================
echo.
echo   Run it any time with:
echo       build\netscanner.exe
echo   or from the Start Menu shortcut "Network IP Scanner".
echo.

choice /C YN /N /M "  Launch Network IP Scanner now? [Y/N] "
if errorlevel 2 goto :end
echo.
build\netscanner.exe

:end
echo.
echo   Goodbye!
endlocal
exit /b 0
