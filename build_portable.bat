@echo off
set "SFML=C:\SFML.backup"
set "GCC=C:\TDM-GCC-64\bin"
set "OUT=BlockPuzzleGame_Portable\BlockPuzzleGame.exe"

if not exist "%GCC%\g++.exe" (
    set "GCC=C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin"
)

echo Compiling updated game directly to portable folder...
"%GCC%\g++.exe" -std=c++17 -O2 ^
    main.cpp ^
    core/GameManager.cpp ^
    core/AIPlayer.cpp ^
    board/Board.cpp ^
    block/Block.cpp ^
    ui/Menu.cpp ^
    ui/HUD.cpp ^
    ui/NetworkClient.cpp ^
    ui/GameNetworkManager.cpp ^
    ui/OnlineGameMode.cpp ^
    ui/OnlineGameSelectScreen.cpp ^
    util/ResourceUtils.cpp ^
    tests/SelfTest.cpp ^
    -I. -Icore -Iboard -Iblock -Iui -Iutil ^
    -I"%SFML%\include" ^
    -L"%SFML%\lib" ^
    -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -lsfml-network ^
    -o %OUT%

if %ERRORLEVEL% NEQ 0 (
    echo Compile failed!
    exit /b 1
)
echo Build success! Updated %OUT%
