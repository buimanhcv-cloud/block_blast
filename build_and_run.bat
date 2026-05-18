@echo off
REM Build and run Block Puzzle Game with Ghost Block Preview

echo Building Block Puzzle Game with Ghost Block Feature...
echo.

if not exist build_x64 mkdir build_x64
cd build_x64

REM Run CMake
echo [1/3] Running CMake...
cmake .. -G "Visual Studio 17 2022" -A x64

REM Build
echo [2/3] Building...
cmake --build . --config Release

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [3/3] Build successful! Launching game...
    echo.
    cd Release
    BlockPuzzleGame.exe
) else (
    echo.
    echo Build failed! Check errors above.
    pause
)
