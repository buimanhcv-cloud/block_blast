@echo off
echo ========================================
echo  Block Puzzle Game - Build Script
echo ========================================

set "SFML=C:\SFML"
set "GCC=C:\TDM-GCC-64\bin"
set "OUT=BlockPuzzleGame.exe"

if not exist "%SFML%" (
    echo ERROR: SFML not found at %SFML%
    echo Please install SFML 2.5 to C:\SFML
    echo Download: https://www.sfml-dev.org/download.php
    pause & exit /b 1
)

if not exist "%GCC%\g++.exe" (
    set "GCC=C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin"
)

echo Compiling...
"%GCC%\g++.exe" -std=c++17 -O2 ^
    main.cpp ^
    core/GameManager.cpp ^
    core/Scoring.cpp ^
    board/Board.cpp ^
    block/Block.cpp ^
    ui/Menu.cpp ^
    ui/DuelSelectScreen.cpp ^
    ui/HUD.cpp ^
    ui/NetworkClient.cpp ^
    ui/GameNetworkManager.cpp ^
    ui/OnlineGameMode.cpp ^
    ui/OnlineGameSelectScreen.cpp ^
    util/ResourceUtils.cpp ^
    util/ScoreStorage.cpp ^
    tests/SelfTest.cpp ^
    -I. -Icore -Iboard -Iblock -Iui -Iutil ^
    -I"%SFML%\include" ^
    -L"%SFML%\lib" ^
    -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -lws2_32 ^
    -o %OUT%

if %ERRORLEVEL% NEQ 0 (
    echo Build FAILED.
    pause & exit /b 1
)

echo Copying DLLs...
copy /Y "%SFML%\bin\sfml-graphics-2.dll" . >nul
copy /Y "%SFML%\bin\sfml-audio-2.dll" . >nul
copy /Y "%SFML%\bin\sfml-window-2.dll" . >nul
copy /Y "%SFML%\bin\sfml-system-2.dll" . >nul
copy /Y "%SFML%\bin\openal32.dll" . >nul

echo.
echo Build SUCCESS: %OUT%
echo Run with: run_windows.bat
pause
