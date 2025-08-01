@echo off
:: Usage: quick_build.bat [args...]
:: This script builds the ATRC GUI using CMake and VS 17 2022

cd /d "%~dp0"
setlocal

if not exist "build" mkdir build
cd build

cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 17 2022" -A x64
cmake --build .

call .\Debug\ATRCGui.exe

endlocal