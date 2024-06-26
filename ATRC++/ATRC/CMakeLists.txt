﻿cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ATRC" C CXX)

# Include GenerateExportHeader module
# include(GenerateExportHeader)

# Create the dynamic-link library (DLL)
add_library (ATRC SHARED 
    "filehandler.cpp"
    "usage.cpp"
    "ATRCFiledata.cpp"
    "include/ATRC.h"
    "include/filer.h"
    "snippets/InsertVar.cpp"
    "snippets/ReadKey.cpp"
    "snippets/ReadVariable.cpp"
    "snippets/DoesExist_var_key_block.cpp"
    "snippets/IsPublic.cpp"
    "snippets/to_save/blocks.cpp"
    "snippets/to_save/variables.cpp"
    "snippets/to_save/keys.cpp"
    )


target_include_directories(ATRC PUBLIC ${CMAKE_BINARY_DIR}/exports)

SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


# Generate export header for the "ATRC" library
# generate_export_header(ATRC EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/ATRC.h)
set_target_properties(ATRC PROPERTIES LINKER_LANGUAGE CXX)

# Set C++ standard (if CMake version is greater than 3.12)
if (CMAKE_VERSION VERSION_GREATER 3.12)
set_property(TARGET ATRC PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
