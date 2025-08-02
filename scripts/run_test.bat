@echo off
setlocal
cd /D "%~dp0\.."

set "PROJECT_ROOT=%cd%"

call :build_dll "x64" ""
if "%errorlevel%"=="1" set ERROR_ENCOUNTERED=1

if "%ERROR_ENCOUNTERED%"=="1" (
    echo One or more errors occurred during the build process.
    exit /b 1
) else (
    echo All builds completed successfully!
    echo Running tests...
    cd /D "%~dp0\.."
    "./out/win_x64/ATRC.Test/Debug/ATRC.Test.exe"
)

exit /b 0

:build_dll
set ARCH=%1
set BUILD_TYPE=%2
set BUILD_DIR=%PROJECT_ROOT%\out\win_%ARCH%

REM Create necessary directories for output
set TARGET_DIR="%OUTPUT_DIR%\Windows\%ARCH%\%BUILD_TYPE%"
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
exit /b 0
endlocal