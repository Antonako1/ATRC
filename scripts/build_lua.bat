@echo off
setlocal
set WSL_DISTRO=debian
cd /D %~dp0
call .\global.bat

cd ..\Wrappers\Lua
if exist build rmdir /S /Q build
mkdir build

rmdir /Q/S libs\ATRC
XCOPY ..\..\ATRC-%VERSION%_%BUILD_VERSION% libs\ATRC /E /I /Y

cmake -S . -B build -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release -A x64
cmake --build build --config Release


set OUTPUT_DIR=%PROJECT_ROOT%\ATRCLua-%VERSION%_%BUILD_VERSION%
mkdir %OUTPUT_DIR%

mkdir %OUTPUT_DIR%\libs
XCOPY libs %OUTPUT_DIR%\libs /E /I /Y
COPY /Y/B build\Release\ATRCLua.dll %OUTPUT_DIR%\libs\ATRCLua.dll

rmdir /S/Q build
mkdir build
call "%WSL_DISTRO%" run "cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release"
copy /Y/B build/libATRCLua.so %OUTPUT_DIR%\libs\libATRCLua.so

mkdir %OUTPUT_DIR%\example
rmdir /S/Q test\libs
XCOPY test %OUTPUT_DIR%\example /E /I /Y


copy /Y .\README.md %OUTPUT_DIR%\README.md
copy /Y .\docs.md %OUTPUT_DIR%\docs.md

7z a -t7z "%PROJECT_ROOT%\ATRCLua-%VERSION%_%BUILD_VERSION%.7z" "%OUTPUT_DIR%\*" -r

endlocal