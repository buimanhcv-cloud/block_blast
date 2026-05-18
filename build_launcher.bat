@echo off
REM Build Game Launcher

if not exist build_launcher mkdir build_launcher
cd build_launcher

REM Run CMake
cmake .. -G "Visual Studio 17 2022" -A x64

REM Build
cmake --build . --config Release

echo.
echo Build complete!
echo GameLauncher.exe should be in: build_launcher\Release\GameLauncher.exe
echo.
pause
