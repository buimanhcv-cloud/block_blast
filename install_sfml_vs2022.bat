@echo off
REM Download and extract pre-built SFML for Visual Studio 2022 x64
REM This script will download SFML 2.6.1 compiled for Visual Studio

echo Downloading SFML 2.6.1 (Visual Studio, x64)...
echo.

REM Create temp directory
if not exist temp_sfml mkdir temp_sfml
cd temp_sfml

REM Download SFML (Visual C++ 2015 version works with VS 2022)
REM Using official SFML release
powershell -Command "Invoke-WebRequest -Uri 'https://github.com/SFML/SFML/releases/download/2.6.1/SFML-2.6.1-windows-vc15-64-bit.zip' -OutFile 'sfml.zip'"

if %ERRORLEVEL% NEQ 0 (
    echo Error downloading SFML. Please check your internet connection.
    pause
    exit /b 1
)

echo Extracting SFML...
powershell -Command "Expand-Archive -Path 'sfml.zip' -DestinationPath '.'"

REM Move extracted SFML to C:\SFML
echo Moving SFML to C:\SFML...
if exist "C:\SFML_backup" rmdir /s /q "C:\SFML_backup"
if exist "C:\SFML" move "C:\SFML" "C:\SFML_backup"
move "SFML-2.6.1" "C:\SFML"

cd ..
rmdir /s /q temp_sfml

echo.
echo Done! SFML 2.6.1 for Visual Studio installed to C:\SFML
echo.
echo Next: Run build_and_run.bat again
pause
