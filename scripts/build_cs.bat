@echo off
setlocal
chcp 65001
cd /D %~dp0
call "global.bat"
cd ..
cd Wrappers\C#

if not exist lib rmdir /S /Q lib
mkdir lib\Windows
mkdir lib\Linux
echo Copying libraries to lib directory...
XCOPY %OUTPUT_DIR%\Windows\* lib\Windows\ /E /I /Y
XCOPY %OUTPUT_DIR%\Linux\* lib\Linux\ /E /I /Y

set PROJECT=ATRCWrapper\ATRCWrapper.csproj

rmdir /S /Q ATRCWrapper\nupkg

set PACKAGE_NAME=ATRC-%VERSION%_%BUILD_VERSION%.nupkg
echo Building package: %PACKAGE_NAME%
dotnet pack ATRCWrapper -c Release /p:Version=%VERSION%
ren ATRCWrapper\nupkg\ATRC.*.nupkg %PACKAGE_NAME%
copy /Y/B ATRCWrapper\nupkg\%PACKAGE_NAME% %PROJECT_ROOT%\%PACKAGE_NAME%
endlocal