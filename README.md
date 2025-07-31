# ATRC - Advanced Tagged Resource Configuration Library

ATRC is a robust **C configuration library**, designed for high-performance applications, game engines, and modular systems.

ATRC has wrappers implemented for:
 - C#. See its [project root](https://github.com/Antonako1/ATRC/tree/main/Wrappers/C%23) or [NuGet gallery](https://www.nuget.org/packages/ATRC/)
 - C++. See its [project root](https://github.com/Antonako1/ATRC/tree/main/Wrappers/C%2B%2B). This wrapper is automatically included in the prebuilt packages as ATRC.hpp
 - Lua. See its [project root](https://github.com/Antonako1/ATRC/tree/main/Lua). Download prebuilt binaries from [Releases](https://github.com/Antonako1/ATRC/releases) or build them yourself. See the README inside Lua directory
 
**[Full Documentation](https://github.com/Antonako1/ATRC/blob/main/docs/)**

---

## Key Features

- **Smart Preprocessor**: Conditional logic, file inclusion, platform detection
- **Dynamic Variables**: Runtime variable injection and substitution
- **Cross-Platform**: Windows and Linux support with architecture detection

---

## Installation

Download prebuilt release and debug binaries for Windows (x64, x86) and Linux (x64) from [Releases](https://github.com/Antonako1/ATRC/releases)

Windows prebuilt binaries are compiled with VS 17 2022

Linux prebuilt 64-bit binaries are compiled with CLang

For building binaries, see [Building](#building) section

---

## Quick Start

### Basic Usage

```c
#include <ATRC.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    ATRC_FD* fd = Create_ATRC_FD("./servercond.atrc", ATRC_READ_ONLY);
    const char *platform = ReadKey("ServerConfig", "Platform");
    const char *env = ReadKey("ServerConfig", "Environment");
    printf("Running %s on %s", platform, env);
    Destroy_ATRC_FD(fd);

    // Alternatively use macros to shorten function calls
    ATRC_FD* fd2 = Create_ATRC_FD("./serverconf.atrc", ATRC_READ_ONLY);
    #define GLB_ATRC_FD fd2
    const char *platform = __READKEY("ServerConfig", "Platform");
    const char *env = __READKEY("ServerConfig", "Environment");
    printf("Running %s on %s", platform, env);
    __DESTROY_ATRC_FD
    #define GLB_ATRC_FD NULL
    
    return 0;
}
```

### Configuration Example

```ini
#!ATRC
%user%=admin
%env%=production

[ServerConfig]
#.IF WINDOWS
Platform=Windows
#.ELSE
Platform=Unix
#.ENDIF

Host=%user%@server.com
Environment=%env%
```

---

## CMake Integration

```bash
cmake_minimum_required(VERSION 3.8)
project(MyProject VERSION 1.0 LANGUAGES C)

add_executable(MyProject "main.c")

# Add ATRC
set(ATRC_DIR "path/to/ATRC-2.3.2_{BUILD}/cmake")
find_package(ATRC REQUIRED)
target_link_libraries(MyProject PRIVATE ATRC::ATRC)
```

---

## Building

Building ATRC requires C++17

```bash
mkdir build
cd build
# If you want to compile the test program, switch "ATRC_BUILD_TESTS" to "ON"
cmake .. -DATRC_BUILD_TESTS=OFF
cmake --build .
```

---

## License

ATRC is open-source under the [BSD-2-Clause license](LICENSE.txt).

Created by [Antonako1](https://github.com/antonako1)

