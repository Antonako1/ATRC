@echo off
cl.exe /I include test.c /link /LIBPATH:lib /OUT:test.exe
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)
del *.obj
echo Compilation successful. Running test...
test.exe