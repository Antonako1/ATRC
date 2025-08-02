# Toolchain file for Linux
message(STATUS "Linux 64-bit toolchain file")

# Specify the compilers
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_LINKER "ld.lld")
set(CMAKE_SYSTEM_NAME Linux)


# Define shared libraries build (ensure SO files are built)
set(BUILD_SHARED_LIBS ON)

# target_compile_definitions(ATRC PRIVATE ATRC_EXPORTS)

set(CMAKE_C_FLAGS_DEBUG "-fPIC -O0 -Wall -Wextra -g")
set(CMAKE_C_FLAGS_RELEASE "-fPIC -O3 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-fPIC -O0 -Wall -Wextra -g")
set(CMAKE_CXX_FLAGS_RELEASE "-fPIC -O3 -Wall -Wextra")


