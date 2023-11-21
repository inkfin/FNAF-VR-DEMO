#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "xr_common/xr_linear.h"

#include "Window/GlfwWindow.h"
#include "Game/GlobalObjects.h"
#include "Objects/Pawn.h"
#include "Objects/SkinnedMesh.h"


class MeshBase;

namespace Scene {

inline float angle;
inline float scale;

inline glm::mat4 Mhmd(1.0f);
inline glm::mat4 Mcontroller[2];
inline float Trigger[2] { 0.0f, 0.0f };

void Display(GLFWwindow* window);
void DisplayVr(const glm::mat4& P, const glm::mat4& V);
void Init();
void Idle();

}