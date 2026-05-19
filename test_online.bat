@echo off
REM Quick test script for online multiplayer mode
REM Start this after: node server.js

echo ========================================
echo  Online PvP Test Setup
echo ========================================
echo.

REM Check if game executable exists
if not exist "BlockPuzzleGame.exe" (
    echo ERROR: BlockPuzzleGame.exe not found!
    echo Please run: build_windows.bat first
    echo.
    pause
    exit /b 1
)

echo Prerequisites:
echo 1. Start server in another terminal: node server.js
echo 2. This script will launch TWO game instances
echo.
pause

echo.
echo Launching Player 1...
start BlockPuzzleGame.exe

echo Waiting 2 seconds...
timeout /t 2 /nobreak

echo Launching Player 2...
start BlockPuzzleGame.exe

echo.
echo Both instances launched!
echo.
echo Instructions:
echo - Click "Online Đối chiến" in each window
echo - Select game type (Realtime or Turn-based)
echo - Enter server IP: 127.0.0.1
echo - Both will wait for opponent
echo - When both connected, game starts!
echo.
pause
