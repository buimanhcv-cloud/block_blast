@echo off
setlocal

:: -------------------------------------------------
:: 1. Ensure required directories exist
:: -------------------------------------------------
if not exist "%~dp0BlockPuzzleGame_Portable" (
    echo Creating portable folder...
    mkdir "%~dp0BlockPuzzleGame_Portable"
)

:: -------------------------------------------------
:: 2. Start Playit tunnel (if not already running)
:: -------------------------------------------------
:: Playit is assumed to be running. If you need to start it:
:: start "" cmd /c "playit.exe serve --local-port 5000 --protocol tcp"
:: (Uncomment above line if you need to launch Playit from batch)

:: -------------------------------------------------
:: 3. Start server (Node) in background if not already running
:: -------------------------------------------------
rem Check if server is already listening on port 5000
netstat -ano | findstr :5000 >nul
if errorlevel 1 (
    echo Starting Block Puzzle server...\r
    start "" cmd /c "node \"%~dp0server.js\""
    timeout /t 3 >nul
) else (
    echo Server already running.
)

:: -------------------------------------------------
:: 4. Build the game binary if missing
:: -------------------------------------------------
if not exist "%~dp0BlockPuzzleGame_Portable\BlockPuzzleGame.exe" (
    echo Building game...\r
    call build_windows.bat
    if errorlevel 1 (
        echo Build failed. Exiting.\r
        exit /b 1
    )
)

:: -------------------------------------------------
:: 5. Launch two client instances
:: -------------------------------------------------
echo Launching Player 1...\r
start "" "%~dp0BlockPuzzleGame_Portable\BlockPuzzleGame.exe"
timeout /t 1 >nul
echo Launching Player 2...\r
start "" "%~dp0BlockPuzzleGame_Portable\BlockPuzzleGame.exe"

:: -------------------------------------------------
:: 6. Show connection info
:: -------------------------------------------------
echo.\r
echo ==========================================================\r
echo  Two clients started. They will automatically connect to:\r
echo  weight-trustless.gl.at.ply.gg:33412\r
echo ==========================================================\r
echo Press any key to close this window...\r
pause >nul
