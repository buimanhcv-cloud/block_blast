@echo off
setlocal
cd /d "%~dp0"

echo ============================================
echo  Block Puzzle Game - Package Portable
echo ============================================
echo.

set "OUT=BlockPuzzleGame_Portable"
set "RELEASE=build_x64\Release"
set "VCRT=C:\BuildTools\VC\Redist\MSVC\14.44.35112\x64\Microsoft.VC143.CRT"

REM ── Check build exists ────────────────────────────────────────────────────
if not exist "%RELEASE%\BlockPuzzleGame.exe" (
    echo ERROR: Build not found. Run build_and_run.bat first.
    pause & exit /b 1
)

REM ── Clean and recreate output folder ─────────────────────────────────────
echo [1/5] Preparing output folder: %OUT%
if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%"
mkdir "%OUT%\assets"

REM ── Copy game exe ─────────────────────────────────────────────────────────
echo [2/5] Copying game executable...
copy /Y "%RELEASE%\BlockPuzzleGame.exe" "%OUT%\" >nul

REM ── Copy SFML DLLs (Release only, not -d debug versions) ─────────────────
echo [3/5] Copying SFML DLLs...
copy /Y "%RELEASE%\sfml-graphics-2.dll"  "%OUT%\" >nul
copy /Y "%RELEASE%\sfml-audio-2.dll"     "%OUT%\" >nul
copy /Y "%RELEASE%\sfml-window-2.dll"    "%OUT%\" >nul
copy /Y "%RELEASE%\sfml-system-2.dll"    "%OUT%\" >nul
copy /Y "%RELEASE%\sfml-network-2.dll"   "%OUT%\" >nul
copy /Y "%RELEASE%\openal32.dll"         "%OUT%\" >nul

REM ── Copy Visual C++ Runtime DLLs ─────────────────────────────────────────
echo [4/5] Copying Visual C++ Runtime DLLs...
if exist "%VCRT%\msvcp140.dll"       copy /Y "%VCRT%\msvcp140.dll"       "%OUT%\" >nul
if exist "%VCRT%\msvcp140_1.dll"     copy /Y "%VCRT%\msvcp140_1.dll"     "%OUT%\" >nul
if exist "%VCRT%\msvcp140_2.dll"     copy /Y "%VCRT%\msvcp140_2.dll"     "%OUT%\" >nul
if exist "%VCRT%\vcruntime140.dll"   copy /Y "%VCRT%\vcruntime140.dll"   "%OUT%\" >nul
if exist "%VCRT%\vcruntime140_1.dll" copy /Y "%VCRT%\vcruntime140_1.dll" "%OUT%\" >nul

REM Fallback: copy from System32 if BuildTools path not found
if not exist "%OUT%\msvcp140.dll" (
    echo    Fallback: copying from System32...
    copy /Y "C:\Windows\System32\msvcp140.dll"       "%OUT%\" >nul 2>&1
    copy /Y "C:\Windows\System32\msvcp140_1.dll"     "%OUT%\" >nul 2>&1
    copy /Y "C:\Windows\System32\vcruntime140.dll"   "%OUT%\" >nul 2>&1
    copy /Y "C:\Windows\System32\vcruntime140_1.dll" "%OUT%\" >nul 2>&1
)

REM ── Copy assets if they exist ─────────────────────────────────────────────
echo [5/5] Copying assets...
if exist "assets\arial.ttf"      copy /Y "assets\arial.ttf"      "%OUT%\assets\" >nul
if exist "assets\place.wav"      copy /Y "assets\place.wav"      "%OUT%\assets\" >nul
if exist "assets\clear.wav"      copy /Y "assets\clear.wav"      "%OUT%\assets\" >nul
if exist "assets\combo.wav"      copy /Y "assets\combo.wav"      "%OUT%\assets\" >nul
if exist "assets\gameover.wav"   copy /Y "assets\gameover.wav"   "%OUT%\assets\" >nul

REM ── Create launcher scripts ───────────────────────────────────────────────
echo Creating launcher scripts...

REM Simple batch launcher
(
echo @echo off
echo start "" "%%~dp0BlockPuzzleGame.exe"
) > "%OUT%\Play.bat"

REM VBS launcher (no console window)
(
echo Set WshShell = CreateObject^("WScript.Shell"^)
echo WshShell.Run Chr^(34^) ^& Replace^(WScript.ScriptFullName, "Play.vbs", "BlockPuzzleGame.exe"^) ^& Chr^(34^), 0
echo Set WshShell = Nothing
) > "%OUT%\Play.vbs"

REM ── List what was packaged ────────────────────────────────────────────────
echo.
echo ============================================
echo  Package complete: %OUT%\
echo ============================================
echo.
dir /b "%OUT%"
echo.
echo Send the entire "%OUT%" folder to other players.
echo They just double-click Play.bat or Play.vbs to run.
echo.
echo NOTE: If the game still fails on their machine,
echo       they need to install:
echo       https://aka.ms/vs/17/release/vc_redist.x64.exe
echo.
pause
