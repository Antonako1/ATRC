cmake_minimum_required(VERSION 3.12)

project("ATRCGui" C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Executable source
add_executable(ATRCGui
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
)

# ATRC lib (assuming find_package works)
set(ATRC_DIR "${CMAKE_SOURCE_DIR}/libs/ATRC/cmake")
find_package(ATRC REQUIRED)
target_link_libraries(ATRCGui PRIVATE ATRC::ATRC)

# GLFW
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/libs/glfw")
set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")
set(GLFW_LIB_DIR "${GLFW_DIR}/lib")  # make sure this matches your directory name

target_include_directories(ATRCGui PRIVATE "${GLFW_INCLUDE_DIR}")
target_link_directories(ATRCGui PRIVATE "${GLFW_LIB_DIR}")
target_link_libraries(ATRCGui PRIVATE glfw3)

# ImGui sources
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/libs/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${GLFW_INCLUDE_DIR}
)

target_link_libraries(imgui PUBLIC glfw3)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC OpenGL::GL)
target_link_libraries(ATRCGui PRIVATE imgui OpenGL::GL)

# Also include ImGui headers in your app
target_include_directories(ATRCGui PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

