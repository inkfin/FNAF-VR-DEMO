#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace GlfwCallbacks
{
    void Register(GLFWwindow* window);
    void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
    void MouseCursor(GLFWwindow* window, double x, double y);
    void MouseButton(GLFWwindow* window, int button, int action, int mods);
    void Resize(GLFWwindow* window, int width, int height);
    void Scroll(GLFWwindow* window, double xoffset, double yoffset);
    void DragAndDrop(GLFWwindow* window, int count, const char** paths);
};