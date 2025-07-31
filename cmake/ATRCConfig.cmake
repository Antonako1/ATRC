# ATRCConfig.cmake
# Usage:
#   set(ATRC_DIR "/path/to/ATRC/cmake")
#   
#   find_package(ATRC REQUIRED)
#   
#   add_executable(MyProject main.cpp)
#   
#   target_link_libraries(MyProject PRIVATE ATRC::ATRC)

set(ATRC_FOUND TRUE)
set(ATRC_VERSION 2.3.2)

set(ATRC_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(ATRC_INCLUDE_DIR "${ATRC_ROOT}/include")

# Determine architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ATRC_ARCH "x64")
else()
    set(ATRC_ARCH "x86")
endif()

# Platform-specific logic
if(WIN32)
    if(${ATRC_ARCH} STREQUAL "x86")
        set(ATRC_ARCH "Win32")
    endif()
    set(ATRC_LIB_DIR "${ATRC_ROOT}/Windows/${ATRC_ARCH}")
    set(ATRC_LIB_DEBUG_DIR "${ATRC_LIB_DIR}/Debug")
    set(ATRC_LIB_RELEASE_DIR "${ATRC_LIB_DIR}/Release")

    set(ATRC_LIB_DEBUG "${ATRC_LIB_DEBUG_DIR}/ATRC.lib")
    set(ATRC_LIB_RELEASE "${ATRC_LIB_RELEASE_DIR}/ATRC.lib")
    set(ATRC_DLL_DEBUG "${ATRC_LIB_DEBUG_DIR}/ATRC.dll")
    set(ATRC_DLL_RELEASE "${ATRC_LIB_RELEASE_DIR}/ATRC.dll")

elseif(UNIX)
    set(ATRC_LIB_DIR "${ATRC_ROOT}/Linux/${ATRC_ARCH}")
    set(ATRC_LIB_DEBUG "${ATRC_LIB_DIR}/Debug/libATRC.so")
    set(ATRC_LIB_RELEASE "${ATRC_LIB_DIR}/Release/libATRC.so")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Create and import targets
include("${CMAKE_CURRENT_LIST_DIR}/ATRCTargets.cmake")

# Export variables
set(ATRC_INCLUDE_DIR "${ATRC_INCLUDE_DIR}")
set(ATRC_LIB_DEBUG "${ATRC_LIB_DEBUG}")
set(ATRC_LIB_RELEASE "${ATRC_LIB_RELEASE}")
set(ATRC_DLL_DEBUG "${ATRC_DLL_DEBUG}")
set(ATRC_DLL_RELEASE "${ATRC_DLL_RELEASE}")
set(ATRC_ARCH "${ATRC_ARCH}")
