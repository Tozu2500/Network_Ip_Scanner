@echo off
setlocal

rem Make sure mingw32-make/g++ are reachable even if not on the system PATH.
where mingw32-make >nul 2>nul
if errorlevel 1 (
    if exist "C:\msys64\mingw64\bin\mingw32-make.exe" (
        set "PATH=C:\msys64\mingw64\bin;%PATH%"
    ) else (
        echo [!] mingw32-make not found. Install MinGW-w64 or add it to PATH.
        pause
        exit /b 1
    )
)

echo Building netscanner...
mingw32-make
if errorlevel 1 (
    echo [!] Build failed.
    pause
    exit /b 1
)

echo.
echo Starting netscanner...
echo.
build\netscanner.exe

endlocal
