add_library(ATRC SHARED 
    "filehandler2.cpp"
    "core_dev.cpp"
    "c_api.c"
    "c_wrapper.cpp"
    "ATRCFiledata.cpp"
    "atrc_stdlib.cpp"
)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

add_compile_definitions(ATRC_EXPORTS)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(ATRC PUBLIC ${CMAKE_BINARY_DIR}/exports)
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
target_include_directories(ATRC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)