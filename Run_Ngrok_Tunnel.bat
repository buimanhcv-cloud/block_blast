@echo off
title Block Blast Ngrok Tunnel
echo ==================================================
echo   Starting Public Ngrok Tunnel for Game Server
echo ==================================================
echo.
echo [!] Warning: Please make sure your Node.js matchmaking server is running.
echo     (If not, run 'run_server.bat' first).
echo.
echo Starting tunnel on port 5000...
echo.

"C:\Users\buima\AppData\Local\Microsoft\WinGet\Packages\Ngrok.Ngrok_Microsoft.Winget.Source_8wekyb3d8bbwe\ngrok.exe" tcp 5000

pause
