@echo off
call C:\BuildTools\Common7\Tools\VsDevCmd.bat >nul
cd /d C:\Users\buima\Documents\app\build
cmake .. -G "NMake Makefiles"
cmake --build . --config Release
