How to use presets

Build and run with CMake. On Linux, use presets with `--preset` found in this directory

```sh
    # Linux
    cmake --preset linux-x64-debug -B./out/linux-x64-debug/build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchain-linux-x64.cmake && cmake --build ./out/linux-x64-debug/build --config Debug
```