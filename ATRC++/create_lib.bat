@ECHO OFF

ECHO TODO:
ECHO ZIP UP WITH THE FOLLOWING TREE:
ECHO LICENSE.txt
ECHO \include\
ECHO --ATRC.h
ECHO \libs\
ECHO --win
ECHO ----ATRC.lib
ECHO ----ATRC.dll
ECHO --linux
ECHO ----ATRC.so
ECHO 

cd /d %~dp0
cd ATRC
RMDIR /S /Q out
cd ..
if exist libs-output RMDIR /S /Q 
setlocal
call .\batch\globals.bat
call .\batch\msvs-dev-env.bat
cd ..\ATRC
call ..\batch\build.bat Release
cd /d %~dp0
if not exist libs-output mkdir libs-output
copy .\ATRC\out\ATRC.dll .\libs-output
copy .\ATRC\out\ATRC.lib .\libs-output
copy .\ATRC\include\ATRC.h .\libs-output
cd ATRC
RMDIR /S /Q out
cd /d %~dp0
endlocal