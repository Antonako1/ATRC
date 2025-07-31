@echo off
setlocal
cd /d "%~dp0"

if not exist "build" mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

cd ..
cd test
if not exist "libs" mkdir libs
copy /Y/B ..\build\Release\*.dll libs\

SET LUA_CPATH=%CD%\libs\ATRCLua.dll;%LUA_CPATH%
SET PATH=%CD%\libs;%PATH%


lua54.exe test.lua
endlocal