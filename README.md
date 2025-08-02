# ATRC - Advanced Tagged Resource Configuration Library

ATRC is a high-performance, extensible configuration library for **C**, with official wrappers for **C++**, **C#**, and **Lua**. Inspired by INI syntax but packed with advanced features, ATRC is ideal for game engines, embedded tools, and modular applications.

**[Documentation](https://github.com/Antonako1/ATRC/blob/main/docs/)** | [VSCode Extension](https://github.com/Antonako1/ATRC-VSCode)

---

## Features

- **Preprocessor Engine**: Conditional logic, includes, raw strings, and logging
- **Variable System**: Global, private, and injected variables
- **Cross-Platform**: Windows and Linux support with architecture flags
- **Lightweight & Fast**: Written in C with minimal dependencies

---

## Language Wrappers

- **C++**: [Wrapper](https://github.com/Antonako1/ATRC/tree/main/Wrappers/C%2B%2B) included in prebuilt packages as `ATRC.hpp`
- **C#**: [Wrapper](https://github.com/Antonako1/ATRC/tree/main/Wrappers/C%23) · [NuGet](https://www.nuget.org/packages/ATRC/)
- **Lua**: [Wrapper](https://github.com/Antonako1/ATRC/tree/main/Wrappers/Lua) · [Releases](https://github.com/Antonako1/ATRC/releases)

---

## Installation

Get prebuilt binaries for Windows (x64/x86) and Linux (x64) from the [Releases](https://github.com/Antonako1/ATRC/releases) page.

Or build from source:
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

To built the release packages, you need to use a Windows computer. See .\scripts\ for more information

---

## CMake Integration

```cmake
cmake_minimum_required(VERSION 3.8)
project(MyProject LANGUAGES C)

add_executable(MyProject main.c)

set(ATRC_DIR "path/to/ATRC-2.4_{BUILD}/cmake")
find_package(ATRC REQUIRED)
target_link_libraries(MyProject PRIVATE ATRC::ATRC)
```

---

## Usage Example

```c
#include <ATRC.h>
#include <stdio.h>

int main() {
    ATRC_FD* fd = Create_ATRC_FD("config.atrc", ATRC_READ_ONLY);
    const char* platform = ReadKey("ServerConfig", "Platform");
    const char* env = ReadKey("ServerConfig", "Environment");
    printf("Running %s on %s\n", platform, env);
    Destroy_ATRC_FD(fd);
    return 0;
}
```

**ATRC Config File:**

```ini
#!ATRC
%env%=production

[ServerConfig]
#.IF WINDOWS
Platform=Windows
#.ELSE
Platform=Unix
#.ENDIF
Environment=%env%
```

---

## License

Licensed under the [BSD-2-Clause license](LICENSE.txt).
Created by [Antonako1](https://github.com/Antonako1)

