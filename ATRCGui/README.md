# ATRC Gui editor for ATRC 2.4_x

Graphical user interface editor for ATRC files

## Building and testing

### Dependencies

 - [ATRC 2.4_x](https://github.com/Antonako1/ATRC/releases/)
 - [ImGUI 1.9.x](https://github.com/ocornut/imgui/releases)
 - OpenGL (Should be automatically included with Windows and Linux)
 - Windows
    - [GLFW 3](https://www.glfw.org/download)
   -
 - Linux
    - GLFW 3. Download development package with your package manager
 - CMake
 - C++ compiler with C++17 support

### Building

#### Windows

1. Download dependencies
2. Create a "libs" directory inside the project root
3. Unzip and paste ATRC-2.4_x into libs. Rename into "ATRC"
4. Unzip and paste the ImGUI source code and paste it into libs. Rename it into "imgui"
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