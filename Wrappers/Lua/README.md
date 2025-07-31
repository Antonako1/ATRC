# Lua wrapper for ATRC

Lua wrapper for ATRC. If your Lua is 64-bit, use 64-bit binaries, if your Lua is 32-bit, use 32-bit binaries
Prebuilt binaries can be downloaded from [Github](https://github.com/Antonako1/ATRC/releases)

ATRCLua.dll is compiled with VS 17 2022.
ATRCLua.so is compiled with GCC

**There are no prebuilt 32-bit binaries!**

## Usage and example(s)

To use ATRC in Lua:

 - Create a "libs" directory
 - Copy ATRC.dll and ATRCLua.dll or libATRC.so and libATRCLua.so to there
 - Add the libs directory into environmental variables "PATH" and "LUA_CPATH"

```lua
local m = require("ATRCLua")

local fd = m.OpenATRC("file.atrc")
local res = ReadKey(fd, "Block", "Key")
print(res)
m.CloseATRC(fd)
```

---

## Building and testing

### Building the Lua bindings

Create a "libs" directory.

[Download](https://sourceforge.net/projects/luabinaries/files/5.4.2/) Lua binaries and extract them into "libs". Rename the Lua binary directory into "Lua".

Download latest release of [ATRC](https://github.com/Antonako1/ATRC/releases/tag/v2.3.2_40) and extract it into "libs". Rename the ATRC directory into "ATRC".

Please check that Lua libraries, CMake and C/C++ compiler can be found in your path


After this run
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

### Testing the bindings

After building the bindings, go to ".\test" and create a directory named "libs".
Copy ATRCLua library from the previous build directory here and the ATRC library from .\libs\ATRC.

Go back inside .\test\libs and add it into LUA_CPATH environmental variable ```SET LUA_CPATH=%CD%\libs\?.dll;%LUA_CPATH%``` on Windows or ```export LUA_CPATH="$(pwd)/?.so;$LUA_CPATH"``` on Linux

To test the Lua bindings, run:
```
cd test
lua54 test.lua
```
