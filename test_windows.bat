@echo off
setlocal

cd /d "%~dp0"

call build_windows.bat
if errorlevel 1 exit /b 1

set "PATH=C:\Program Files\Git\mingw64\bin;C:\SFML\bin;%PATH%"
BlockPuzzleGame.exe --self-test
set "EXIT_CODE=%ERRORLEVEL%"
if not "%EXIT_CODE%"=="0" pause
exit /b %EXIT_CODE%
