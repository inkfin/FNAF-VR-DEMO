#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace DrawGui {

void Init();
void Display(GLFWwindow* window);
extern bool HideGui;

}