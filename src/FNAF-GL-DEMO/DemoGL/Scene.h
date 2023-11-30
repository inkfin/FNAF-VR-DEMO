//
// Created by 11096 on 11/19/2023.
//

#pragma once

#include <Objects/StaticMesh.h>
#include <Game/GlobalObjects.h>

namespace Scene {

inline GLuint fish_texture_id; // Texture map for mesh

void Display(GLFWwindow* window);

// Initialize OpenGL state. This function only gets called once.
void Init();
void Idle();

}