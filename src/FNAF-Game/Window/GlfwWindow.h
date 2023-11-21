#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace GlfwWindow {

extern GLFWwindow* window;
extern const int InitSize[2];
extern int Size[2];
extern float Aspect;
void Create();
void OpenGlCaps();

};