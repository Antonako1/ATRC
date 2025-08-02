@echo off
cd /D %~dp0
setlocal

rmdir /S /Q "build"
mkdir build
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
cmake --build .

endlocal