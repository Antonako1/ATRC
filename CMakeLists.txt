cmake_minimum_required(VERSION 3.12)

project("ATRC" C CXX)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory("ATRC")

# Set manually if argument is not working
set(ATRC_BUILD_TESTS ON CACHE BOOL "Build tests for ATRC library")


if(${ATRC_BUILD_TESTS} STREQUAL "ON")
    add_subdirectory("ATRC.Test")
else()
    message(STATUS "ATRC_BUILD_TESTS is not defined. Skipping ATRC.Test project. You can set this variable on manually inside ${CMAKE_CURRENT_LIST_FILE}")
endif()
