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
rmdir /S /Q "%OUTPUT_DIR%"
mkdir %OUTPUT_DIR%

mkdir %OUTPUT_DIR%\libs
mkdir %OUTPUT_DIR%\libs\ATRCLua
mkdir %OUTPUT_DIR%\libs\ATRC

COPY /Y/B build\Release\ATRCLua.dll %OUTPUT_DIR%\libs\ATRCLua\ATRCLua.dll

rmdir /S/Q build
mkdir build
call "%WSL_DISTRO%" run "cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release"

:: Copy using powershell for Linux compatibility
powershell -Command "Copy-Item -Path build/ATRCLua.so -Destination '%OUTPUT_DIR%\libs\ATRCLua\ATRCLua.so'"


mkdir %OUTPUT_DIR%\example
rmdir /S/Q test\libs
XCOPY test %OUTPUT_DIR%\example /E /I /Y


copy .\libs\ATRC\Linux\x64\Release\ATRC.so %OUTPUT_DIR%\libs\ATRC\ATRC.so
copy .\libs\ATRC\Windows\x64\Release\ATRC.dll %OUTPUT_DIR%\libs\ATRC\ATRC.dll

copy /Y .\README.md %OUTPUT_DIR%\README.md

mkdir %OUTPUT_DIR%\docs
copy /Y .\docs.md %OUTPUT_DIR%\docs\lua_api.md
copy /Y %PROJECT_ROOT%\docs\*.md %OUTPUT_DIR%\docs\

copy /Y %PROJECT_ROOT%\LICENSE.txt %OUTPUT_DIR%\LICENSE.txt
copy /Y %PROJECT_ROOT%\project\CHANGELOG.txt %OUTPUT_DIR%\CHANGELOG.txt
copy /Y %PROJECT_ROOT%\project\VERSION %OUTPUT_DIR%\VERSION.txt
copy /Y %PROJECT_ROOT%\project\BUILDNUMBER %OUTPUT_DIR%\BUILDNUMBER.txt

7z a -t7z "%PROJECT_ROOT%\ATRCLua-%VERSION%_%BUILD_VERSION%.7z" "%OUTPUT_DIR%\*" -r

endlocal