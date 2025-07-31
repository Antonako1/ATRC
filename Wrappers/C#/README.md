# ATRC C#

ATRC C# wrapper

## Using the library

Download from [nuget](https://www.nuget.org/packages/ATRC)

The ATRC native C DLLs need to be copied into the same directory as the executable or added to path for this C# wrapper to work.

All of the C DLLs (ATRC.dll, ATRC.so) can be all copied there, since the library automatically resolves which one to use

Downloads these prebuilt binaries from ATRC's [release page](https://github.com/Antonako1/ATRC/releases). Please check that the version numbers match your current nuget package version

## Docs
ATRC automatically compiles C# docs and should be seen in your IDE automatically when inspecting functions

ATRC Official documentation can be found on the project's github page at [https://github.com/Antonako1/ATRC/tree/main/docs](https://github.com/Antonako1/ATRC/tree/main/docs)

## Building and testing

To build the wrapper, create a "lib" directory inside this directory and paste Linux and Windows directories from the ATRC-<version>_<build_number> there

To build the library, please use <project_root>\scripts\build_cs.bat or run:
```bash
# Version: Take from <project_root>\VERSION
# Build Version: Take from <project_root>\BUILDNUMBER
dotnet publish ATRCWrapper -c Release /p:ATRCVersion="<version>" /p:ATRCBuildVersion="<build_version>"
```

To run the test program, use:
```bash
dotnet run --project ATRCTest
```