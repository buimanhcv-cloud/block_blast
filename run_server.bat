@echo off
echo ========================================
echo  Block Puzzle Game - Online Server
echo ========================================
echo.

REM Check if Node.js is installed
where node >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Node.js not found
    echo Please install Node.js from https://nodejs.org
    pause
    exit /b 1
)

echo Starting server on port 9000...
echo.
echo Waiting for players to connect...
echo.

node server.js

pause
