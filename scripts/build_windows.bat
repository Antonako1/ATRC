@echo off
cd /D %~dp0

call "global.bat"

cd ..

set ERROR_ENCOUNTERED=0

REM Define configurations for Windows builds
REM Do NOT change the order of these configurations
call :build Win32 Debug
if errorlevel 1 set ERROR_ENCOUNTERED=1

call :build Win32 Release
if errorlevel 1 set ERROR_ENCOUNTERED=1

call :build x64 Debug
if errorlevel 1 set ERROR_ENCOUNTERED=1

call :build x64 Release
if errorlevel 1 set ERROR_ENCOUNTERED=1

if "%ERROR_ENCOUNTERED%"=="1" (
    echo One or more errors occurred during the build process.
    exit /b 1
) else (
    echo All builds completed successfully!
)
exit /b 0

REM Build function
:build


set ARCH=%1
set BUILD_TYPE=%2
set BUILD_DIR=%PROJECT_ROOT%\out\win_%ARCH%_%BUILD_TYPE%

REM Create necessary directories for output
set TARGET_DIR=%OUTPUT_DIR%\Windows\%ARCH%\%BUILD_TYPE%
if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"

REM Run CMake commands
cmake -S . -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -G "Visual Studio 17 2022" -A %ARCH%
if errorlevel 1 (
    echo CMake configuration failed for %ARCH% %BUILD_TYPE%.
    exit /b 1
)

cmake --build %BUILD_DIR% --config %BUILD_TYPE%
if errorlevel 1 (
    echo Build failed for %ARCH% %BUILD_TYPE%.
    exit /b 1
)

echo Build succeeded for %ARCH% %BUILD_TYPE%.

REM Copy files to the output directory
echo Copying files to output directory...
set SOURCE_DIR=%BUILD_DIR%\ATRC\%BUILD_TYPE%
COPY /Y/B "%SOURCE_DIR%\ATRC.lib" "%TARGET_DIR%\ATRC.lib"
COPY /Y/B "%SOURCE_DIR%\ATRC.exp" "%TARGET_DIR%\ATRC.exp"
COPY /Y/B "%SOURCE_DIR%\ATRC.dll" "%TARGET_DIR%\ATRC.dll"

if "%BUILD_TYPE%"=="Debug" (
    COPY /Y/B "%SOURCE_DIR%\ATRC.pdb" "%TARGET_DIR%\ATRC.pdb"
)

exit /b 0
