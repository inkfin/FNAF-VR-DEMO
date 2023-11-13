#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "xr_common/xr_linear.h"
#include "ImGui.h"
#include <iostream>

namespace Camera {

void UpdateP();
void UpdateV(glm::vec3 delta_pos);

}

namespace Scene {

extern bool CaptureGui;
extern bool RecordingBuffer;
extern bool ClearDefaultFb;

extern glm::mat4 Mhmd;
extern glm::mat4 Mcontroller[2];
extern float Trigger[2];

void Display(GLFWwindow* window);
void DisplayVr(const glm::mat4& P, const glm::mat4& V);
void Idle();
void Init();

};