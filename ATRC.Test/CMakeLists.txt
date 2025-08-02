cmake_minimum_required(VERSION 3.12)


if(NOT WIN32)
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
    set(CMAKE_LINKER "ld.lld")
    set(CMAKE_LINKER_FLAGS "-v")
endif()

project(ATRC.Test)

# Add executable for testing
add_executable(ATRC.Test
    "ATRC.Test.c"
    "ATRC.Test.cpp"
    "ATRC.Test.h"
)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()



# Set include directories (if needed)

message(STATUS "SOURCE DIR: ${PROJECT_SOURCE_DIR}")
set(ATRC_DIR "${PROJECT_SOURCE_DIR}/libs/ATRC/cmake")
find_package(ATRC REQUIRED)
message(STATUS "ATRC_INCLUDE_DIR = ${ATRC_INCLUDE_DIR}")
message(STATUS "ATRC_LIB_RELEASE = ${ATRC_LIB_RELEASE}")
message(STATUS "ATRC_LIB_DEBUG = ${ATRC_LIB_DEBUG}")
message(STATUS "ATRC_DLL_RELEASE = ${ATRC_DLL_RELEASE}")
message(STATUS "ATRC_DLL_DEBUG = ${ATRC_DLL_DEBUG}")
target_link_libraries(ATRC.Test PRIVATE ATRC::ATRC)

# Set C++ standard for tests (if CMake version is greater than 3.12)
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

