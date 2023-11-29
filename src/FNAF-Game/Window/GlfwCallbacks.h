#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace GlfwCallbacks {

// mouse logic
inline bool captureCursor = false;
inline bool firstMouseEnter = true;

inline double mouse_last_x = 0.0f;
inline double mouse_last_y = 0.0f;

void Register(GLFWwindow* window);
void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseCursor(GLFWwindow* window, double x, double y);
void MouseButton(GLFWwindow* window, int button, int action, int mods);
void MouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void WindowResize(GLFWwindow* window, int width, int height);
void DragAndDrop(GLFWwindow* window, int count, const char** paths);

};