// dear imgui: Platform Backend for VR
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan, WebGPU..)
// (Info: GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

// Implemented features:
//  [X] Platform: Clipboard support.
//  [X] Platform: Keyboard support. Since 1.87 we are using the io.AddKeyEvent() function. Pass ImGuiKey values to all key functions e.g. ImGui::IsKeyPressed(ImGuiKey_Space). [Legacy GLFW_KEY_* values will also be supported unless IMGUI_DISABLE_OBSOLETE_KEYIO is set]
//  [X] Platform: Gamepad support. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange' (note: the resizing cursors requires GLFW 3.4+).

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// About GLSL version:
// The 'glsl_version' initialization parameter defaults to "#version 150" if NULL.
// Only override if your GL version doesn't handle this GLSL version. Keep NULL if unsure!

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

struct GLFWwindow;
struct GLFWmonitor;

IMGUI_IMPL_API bool     ImGui_ImplVr_InitForOpenGL(unsigned int tex);
IMGUI_IMPL_API bool     ImGui_ImplVr_InitForVulkan();
IMGUI_IMPL_API bool     ImGui_ImplVr_InitForOther();
IMGUI_IMPL_API void     ImGui_ImplVr_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplVr_NewFrame();
IMGUI_IMPL_API void		ImGui_ImplVr_EndFrame();

