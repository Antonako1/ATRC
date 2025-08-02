@echo off
pushd %cd%
cd /D %~dp0
cd ..

:: Project root directory
set PROJECT_ROOT=%cd%
set FOLDER=ATRC
set OUTPUT_DIR=%PROJECT_ROOT%\%FOLDER%
SET PROJECT_FOLDER="%PROJECT_ROOT%\project"

SET /P VERSION=< "%PROJECT_FOLDER%\VERSION"

:: Folder name: ATRC-<version>_<build_version>
set FOLDER=%FOLDER%-%VERSION%

if "%1" == "DEL" (
    for /d %%D in ("%PROJECT_ROOT%\%FOLDER%_*") do (
        RMDIR /S /Q "%%D"
    )
    for /d %%D in ("%PROJECT_ROOT%\ATRCLua-%VERSION%_*") do (
        RMDIR /S /Q "%%D"
    )
    del "*.zip"
    del "*.tar.gz"
    del "*.7z"
    del "*.nupkg"

    @REM if exist "out" rmdir /S /Q "out"
)


REM Increment build version
SET BUILD_VERSION=0
REM Add contents of BUILDNUMBER file to BUILD_VERSION
set /P BUILD_VERSION=< "%PROJECT_FOLDER%\BUILDNUMBER"
if "%2" == "INC" (
    set /A BUILD_VERSION+=1
)
REM Write the new build version back to BUILDNUMBER file
echo %BUILD_VERSION% > "%PROJECT_FOLDER%\BUILDNUMBER"

REM Turn decimal build version into a hexadecimal string using PowerShell
FOR /F "delims=" %%i IN ('powershell -Command "[Convert]::ToString(%BUILD_VERSION%, 16)"') DO SET BUILD_VERSION=%%i
REM Ensure the BUILD_VERSION is uppercase
SET BUILD_VERSION=%BUILD_VERSION:~0,8%
REM Update the folder name with the new build version
set FOLDER=%FOLDER%_%BUILD_VERSION%
REM Update the output directory with the new folder name
set OUTPUT_DIR=%PROJECT_ROOT%\%FOLDER%

if "%1" == "DEL" (
    REM Clear the previous output folder
    RMDIR /S /Q "%PROJECT_ROOT%/ATRC-*"
    RMDIR /S /Q "%PROJECT_ROOT%/ATRCLua-*"
    DEL /Q/S/F %FOLDER%.zip
    DEL /Q/S/F %FOLDER%.tar
    DEL /Q/S/F %FOLDER%.tar.gz
    DEL /Q/S/F %FOLDER%.7z

    cd /D %~dp0
    cd Wrappers\Lua
    if exist build rmdir /S /Q build
    if exist libs\ATRC rmdir /S /Q libs\ATRC

    cd ..\C#
    if exist ATRCTest\bin rmdir /S /Q ATRCTest\bin
    if exist ATRCTest\obj rmdir /S /Q ATRCTest\obj

    if exist ATRCWrapper\bin rmdir /S /Q ATRCWrapper\bin
    if exist ATRCWrapper\obj rmdir /S /Q ATRCWrapper\obj
    if exist ATRCWrapper\nupkg rmdir /S /Q ATRCWrapper\nupkg
)
popd