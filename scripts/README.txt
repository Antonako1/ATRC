About the scripts

global.bat
    - Initializes variables for the packaging
    - Args: global.bat [DEL] [INC]
        - First argument: If set to DEL, previous packages will be deleted
        - Second argument: If set to INC, BUILDNUMBER will be incremented

ATRC:

build_and_package.bat
    - Builds the release package
    - Following are required to be installed and found in the path: CMake, 7z, WSL distro, C/C++ compiler
    - WSL distro can be changed inside the script. Default: debian
    - WSL Distro should have CMake, Ninja and CLang installed


build_linux.sh
    - Builds ATRC into a Linux release package
    - Following are required to be installed and found in the path: CMake and CLang

build_windows.bat
    - Builds ATRC into a Windows release package
    - Following are required to be installed and found in the path: CMake, MSVC

run_test.bat
    - Runs the C/C++ test project
    - Following are required to be installed and found in the path: CMake, 7z, C/C++ compiler



Wrappers:

build_cs.bat
    - Builds C# project into a nuget package
    - Following are required to be installed and found in the path: dotnet sdk 8.0, nuget

build_lua.bat
    - Builds Lua wrapper into its own release package
    - Following are required to be installed and found in the path: CMake, 7z, debian, C/C++ compiler
    - WSL distro can be changed inside the script. Default: debian
    - WSL Distro should have CMake, Ninja and C compiler installed
    - See Wrappers\Lua\README.md for prequsities on the libraries that need to be found here


Tools:

build_gui.bat
    - Build GUI into its own release package
    - Following are required to be installed and found in the path: CMake, 7z, debian, C/C++ compiler
    - WSL distro can be changed inside the script. Default: debian
    - WSL Distro should have CMake, Ninja and C compiler installed