:: Following required in PATH
:: A WSL distribution (e.g., Debian)
:: CMake
:: 7z (7-Zip)
:: C/C++ compiler (e.g., GCC for Linux, MSVC for Windows)

@echo off
setlocal
chcp 65001
REM Define the project root and output folder
cd /D %~dp0

SET "WSL_DISTRO=debian"

call "global.bat" DEL INC

cd ..

REM Define subdirectories for the file structure
mkdir "%OUTPUT_DIR%\cmake"
mkdir "%OUTPUT_DIR%\docs"
mkdir "%OUTPUT_DIR%\include"
mkdir "%OUTPUT_DIR%\Linux\x64\Debug"
mkdir "%OUTPUT_DIR%\Linux\x64\Release"
mkdir "%OUTPUT_DIR%\Windows\x64\Debug"
mkdir "%OUTPUT_DIR%\Windows\x64\Release"
mkdir "%OUTPUT_DIR%\Windows\Win32\Release"
mkdir "%OUTPUT_DIR%\Windows\Win32\Debug"


REM Compile Windows builds
cd "%PROJECT_ROOT%\scripts"

call build_windows.bat
if errorlevel 1 exit /b 1

REM Run Linux builds using WSL or similar
ECHO Building for Linux...
cd "%PROJECT_ROOT%\scripts"
call %WSL_DISTRO% run "./build_linux.sh"

REM Copy include files
copy "%PROJECT_ROOT%\ATRC\include\ATRC.h" "%OUTPUT_DIR%\include\ATRC" /Y

REM Copy CMake configuration
copy "%PROJECT_ROOT%\cmake\ATRCConfig.cmake" "%OUTPUT_DIR%\cmake" /Y

REM Copy CMake targets
copy "%PROJECT_ROOT%\cmake\ATRCTargets.cmake" "%OUTPUT_DIR%\cmake" /Y

REM Copy README
copy "%PROJECT_ROOT%\README.md" "%OUTPUT_DIR%" /Y

REM Copy VERSION file
copy "%PROJECT_ROOT%\project\VERSION" "%OUTPUT_DIR%" /Y
REM Copy BUILDNUMBER file
copy "%PROJECT_ROOT%\project\BUILDNUMBER" "%OUTPUT_DIR%" /Y
REM Copy LICENSE file
copy "%PROJECT_ROOT%\LICENSE" "%OUTPUT_DIR%" /Y
REM Copy docs folder
xcopy "%PROJECT_ROOT%\docs" "%OUTPUT_DIR%\docs" /E /I /Y
REM Copy CHANGELOG.txt
copy "%PROJECT_ROOT%\project\CHANGELOG.txt" "%OUTPUT_DIR%" /Y
REM Copy README.md
copy "%PROJECT_ROOT%\README.md" "%OUTPUT_DIR%" /Y



ECHO Starting C++ packaging...
copy "%PROJECT_ROOT%\Wrappers\C++\include\ATRC.hpp" "%OUTPUT_DIR%\include" /Y
ECHO C++ package can be found in "%OUTPUT_DIR%\ATRC.hpp"

REM Compress the output folder into various formats
7z a -t7z "%PROJECT_ROOT%\%FOLDER%.7z" "%OUTPUT_DIR%\*" -r                  > NUL

REM Completion message
echo All tasks completed successfully!



ECHO Starting NUGET packaging...
call "%PROJECT_ROOT%\scripts\build_cs.bat"
echo NUGET package can be found in "%PROJECT_ROOT%\ATRC-%VERSION%_%BUILDNUMBER%.nupkg"

ECHO Compiling Lua wrapper...
call "%PROJECT_ROOT%\scripts\build_lua.bat"
ECHO Lua wrapper can be found in "%PROJECT_ROOT%\ATRCLua-%VERSION%_%BUILDNUMBER%"

endlocal
