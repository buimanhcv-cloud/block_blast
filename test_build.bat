@echo off
echo ========================================
echo  Building and Testing Block Puzzle Game
echo ========================================

cd /d c:\Users\buima\Documents\app

echo.
echo Running build_windows.bat...
call build_windows.bat

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable: BlockPuzzleGame.exe
echo.
echo To test the build:
echo 1. Run: run_windows.bat (for single player)
echo 2. Or start server: node server.js
echo 3. Then run game.exe twice for online multiplayer
echo.
pause
