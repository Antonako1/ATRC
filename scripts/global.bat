@echo off
pushd %cd%
cd /D %~dp0
cd ..

set PROJECT_ROOT=%cd%
set FOLDER=ATRC
set OUTPUT_DIR=%PROJECT_ROOT%\%FOLDER%
SET PROJECT_FOLDER="%PROJECT_ROOT%\project"
set /P VERSION=< "%PROJECT_FOLDER%\VERSION"

set FOLDER=%FOLDER%-%VERSION%

if "%1" == "DEL" (
    for /d %%D in ("%PROJECT_ROOT%\%FOLDER%_*") do (
        RMDIR /S /Q "%%D"
    )
    for /d %%D in ("%PROJECT_ROOT%\ATRCLua_*") do (
        RMDIR /S /Q "%%D"
    )
    del "*.zip"
    del "*.tar.gz"
    del "*.7z"
    del "*.nupkg"
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
    DEL /Q/S/F %FOLDER%.zip
    DEL /Q/S/F %FOLDER%.tar
    DEL /Q/S/F %FOLDER%.tar.gz
    DEL /Q/S/F %FOLDER%.7z
)
popd