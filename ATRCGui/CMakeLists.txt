cmake_minimum_required(VERSION 3.12)

project("ATRCGui" C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Executable source
add_executable(ATRCGui
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/ATRCGui.cpp"
)

# ATRC library
add_subdirectory("${CMAKE_SOURCE_DIR}/../ATRC" "${CMAKE_BINARY_DIR}/ATRC-build")
target_link_libraries(ATRCGui PRIVATE ATRC)

# GLFW
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/libs/glfw")
set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")
set(GLFW_LIB_DIR "${GLFW_DIR}/lib")

target_include_directories(ATRCGui PRIVATE "${GLFW_INCLUDE_DIR}")
target_link_directories(ATRCGui PRIVATE "${GLFW_LIB_DIR}")
target_link_libraries(ATRCGui PRIVATE glfw3)

# Nuklear
set(NUKLEAR_DIR "${CMAKE_SOURCE_DIR}/libs/nuklear")
set(NUKLEAR_INCLUDE_DIR "${NUKLEAR_DIR}/include")

target_include_directories(ATRCGui PRIVATE "${NUKLEAR_INCLUDE_DIR}")

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(ATRCGui PRIVATE OpenGL::GL)

# Project includes
target_include_directories(ATRCGui PRIVATE "${CMAKE_SOURCE_DIR}/include")
