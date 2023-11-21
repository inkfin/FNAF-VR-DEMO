// Imgui headers for UI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "DebugCallback.h"
#include "DemoGL/Scene.h"

// C++ programs start executing in the main() function.
int main(void)
{
    GlfwWindow::Create();

    glewInit();

    glClearColor(0.184313729f, 0.309803933f, 0.309803933f, 1.0f);
    RegisterDebugCallback();


    GlfwWindow::OpenGlCaps();

    Scene::Init();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(GlfwWindow::window)) {
        Scene::Idle();
        Scene::Display(GlfwWindow::window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup Shaders
    Shader::ClearAllShaders();

    glfwTerminate();
    return 0;
}
