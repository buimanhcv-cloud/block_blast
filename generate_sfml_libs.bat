@echo off
REM Generate SFML .lib files from .dll files for Visual Studio
REM Requires: Microsoft Visual Studio with lib.exe in PATH

setlocal enabledelayedexpansion

set SFML_BIN=C:\SFML\bin
set SFML_LIB=C:\SFML\lib
set OUTPUT_DIR=%SFML_LIB%\msvc

if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

echo Generating SFML import libraries for Visual Studio...
echo.

REM Find lib.exe
for /f "tokens=*" %%i in ('where lib.exe 2^>nul') do set LIB_EXE=%%i

if "!LIB_EXE!"=="" (
    echo Error: lib.exe not found in PATH
    echo Please ensure Visual Studio is properly installed
    pause
    exit /b 1
)

echo Using lib.exe: !LIB_EXE!
echo.

REM Generate .lib files for each SFML DLL
for %%D in (sfml-system-2 sfml-window-2 sfml-graphics-2 sfml-audio-2) do (
    if exist "%SFML_BIN%\%%D.dll" (
        echo Generating %%D.lib...
        "!LIB_EXE!" /def /name:%%D.dll /out:"%OUTPUT_DIR%\%%D.lib" /machine:x64
    )
)

echo.
echo Done! Import libraries generated in: %OUTPUT_DIR%
echo.
echo Next step: Update CMakeLists.txt to use these libraries
pause
