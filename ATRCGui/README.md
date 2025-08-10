# ATRC Gui editor for ATRC 3.0_x

Graphical user interface editor for ATRC files

## Building and testing

### Dependencies

 - [ATRC 3.0_x](https://github.com/Antonako1/ATRC/releases/)
 - [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear)
 - OpenGL (Should be automatically included with Windows and Linux)
 - Windows
    - [GLFW 3](https://www.glfw.org/download)
 - Linux
    - GLFW 3. Download development package with your package manager
 - CMake
 - C compiler

### Building

#### Windows

1. Download dependencies
2. Create a "libs" directory inside the project root
3. Unzip and paste ATRC-3.0_x into libs. Rename into "ATRC"
4. Copy Nuklear.h into libs: "nuklear/include/Nuklear/Nuklear.h"
5. Unzip and paste GLFW into libs. Rename into "glfw". Choose your preferred binary directory and rename it into "lib"
6. Unzip and paste SDL into libs. Rename it into sdl.
7. Build with CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Testing