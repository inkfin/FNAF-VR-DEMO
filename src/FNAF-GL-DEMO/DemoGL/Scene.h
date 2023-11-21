//
// Created by 11096 on 11/19/2023.
//

#pragma once

#include <Objects/SkinnedMesh.h>

#include <Game/GlobalObjects.h>

namespace Scene {

// mouse logic
inline bool captureCursor = false;
inline bool firstMouseEnter = true;

inline double mouse_last_x = 0.0f;
inline double mouse_last_y = 0.0f;

inline GLuint fish_texture_id; // Texture map for mesh

void Display(GLFWwindow* window);

// Initialize OpenGL state. This function only gets called once.
void Init();
void Idle();

}