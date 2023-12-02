//
// Created by 11096 on 12/2/2023.
//

#pragma once

#include <memory>
#include "Shader.h"

namespace LightManager {

struct PointLightUniforms {
    glm::vec3 position = glm::vec3(0.0f, 2.0f, 1.0f);
    glm::vec3 La = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 Ld = glm::vec3(0.7f, 0.2f, 0.8f);
    glm::vec3 Ls = glm::vec3(0.8f, 0.8f, 0.8f);
    float constant = 1.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;
};
#define POINT_LIGHT_COUNT 2
inline PointLightUniforms pointLightData[POINT_LIGHT_COUNT];

struct DirLightUniforms {
    glm::vec3 position = glm::vec3(0.0f, -2.0f, 1.0f);
    glm::vec3 La = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Ld = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Ls = glm::vec3(0.0f, 0.0f, 0.0f);
};
inline DirLightUniforms dirLightData;

struct SpotLightUniforms {
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
    float cutOff = 1.0f;

    glm::vec3 position = glm::vec3(0.0f, 4.0f, 0.0f);
    glm::vec3 La = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 Ld = glm::vec3(0.7f, 0.2f, 0.8f);
    glm::vec3 Ls = glm::vec3(0.8f, 0.8f, 0.8f);

    float constant = 1.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;
};
inline SpotLightUniforms spotLightData;

inline bool use_flash_light = true;

void InitLight();

void SetLightUniforms(const Shader* pShader);

};

