cmake_minimum_required(VERSION 3.12)

project("ATRCLua" C)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_library(ATRCLua SHARED 
    "src/lua_bindings.c"
)


if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

add_compile_definitions(ATRC_EXPORTS)

# include the ATRC library
set(ATRC_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/ATRC/cmake")
find_package(ATRC REQUIRED)
target_link_libraries(ATRCLua PRIVATE ATRC::ATRC)
if(WIN32)
    add_custom_command(TARGET ATRCLua POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ATRC_DLL_RELEASE}" $<TARGET_FILE_DIR:ATRCLua>
        COMMENT "Copying ATRC DLL to output directory"
    )
endif()

# Include Lua
set(LUA_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/Lua/")

if(WIN32)
    target_link_libraries(ATRCLua PRIVATE "${LUA_DIR}/lua54.lib")
else()
    target_link_libraries(ATRCLua PRIVATE "${LUA_DIR}/liblua54.a")
endif()

target_include_directories(ATRCLua PRIVATE 
    "${LUA_DIR}/include"
)