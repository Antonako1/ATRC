/*+++
This project is licensed under the BSD 2-Clause License.
See the LICENSE file in the project root for license information.

Graphical user interface editor for ATRC files.

Author(s): 
    Antonako1

Maintained at https://github.com/Antonako1/ATRC/ATRCGui
---*/
#pragma once
#ifndef ATRCGUI_H
#define ATRCGUI_H
#include "ATRCMem.hpp"

#ifndef __cplusplus
#error "ATRCGui requires C++ support. Please use a C++ compiler."
#endif // __cplusplus

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#define GLSL_VERSION "#version 130"

class ATRCGui {
public:
    ATRCGui();
    ~ATRCGui();

private:
    GLFWwindow* _window;
    ImGuiContext* _imguiContext;
};

#endif // ATRCGUI_H