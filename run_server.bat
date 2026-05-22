@echo off
title Block Blast Matchmaking Server
:loop
echo [%date% %time%] Starting Block Blast Matchmaking Server...
node server.js
echo.
echo [%date% %time%] Server crashed or stopped. Auto-restarting in 3 seconds...
timeout /t 3 /nobreak >nul
goto loop
