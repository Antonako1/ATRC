@ECHO OFF
:: Set the starting directory
setlocal
set "VSCMD_START_DIR=%CD%"
cd /d "%VSCMD_START_DIR%"
call .\globals.bat
@rem not needed
if not defined DevEnvDir ( 
    call "%TOOLSPATH%\VsDevCmd.bat"
)
:: Restore the starting directory
cd /d "%VSCMD_START_DIR%"

:: Build the library
cd ATRC
call ..\build.bat "Debug"
cd ..

:: Build the test

cd ATRC.Test
if not exist .\libs mkdir .\libs
COPY /Y /B ..\ATRC\out\ATRC.dll .\libs\
COPY /Y /B ..\ATRC\out\ATRC.lib .\libs\

@REM Temporary location at .\libs\
if not exist .\libs\include mkdir .\libs\include
COPY /Y ..\ATRC\include\ATRC.h .\libs\include
if not exist .\out mkdir .\out
call ..\build.bat "Debug"
call .\run.bat
