@echo off
call C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat x64 >nul
cd /d C:\Users\buima\Documents\app\build_vs2022
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
