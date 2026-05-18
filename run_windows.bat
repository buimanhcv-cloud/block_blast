@echo off
setlocal

cd /d "%~dp0"
echo Running from %CD%

if not exist BlockPuzzleGame.exe (
    call build_windows.bat
    if errorlevel 1 exit /b 1
)

set "PATH=C:\Program Files\Git\mingw64\bin;C:\SFML\bin;%PATH%"
echo Launching BlockPuzzleGame.exe
BlockPuzzleGame.exe
set "EXIT_CODE=%ERRORLEVEL%"
echo Process exited with code %EXIT_CODE%
if not "%EXIT_CODE%"=="0" pause
exit /b %EXIT_CODE%
