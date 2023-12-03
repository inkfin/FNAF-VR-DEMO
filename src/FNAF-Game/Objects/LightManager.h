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

    bool isOn = true;
};
#define POINT_LIGHT_COUNT 4
inline PointLightUniforms pointLightData[POINT_LIGHT_COUNT];

struct DirLightUniforms {
    glm::vec3 position = glm::vec3(0.0f, -2.0f, 1.0f);
    glm::vec3 La = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Ld = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Ls = glm::vec3(0.0f, 0.0f, 0.0f);

    bool isOn = true;
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

    // use_flash_light outside of this struct
};
inline SpotLightUniforms spotLightData;

inline bool use_flash_light = false;

void InitLight();

void SetLightUniforms(const Shader* pShader);

// light control functions
void LightOn();
void LightOff();
void LightSuccess();
void LightFailure();

void LightMarquee(float deltaTime, float interval = 0.5f);

void LightGlitter(float deltaTime, float on_interval_max = 0.2f, float on_interval_min = 0.01f, float off_interval_max = 0.2f, float off_interval_min = 0.01f);

// light sequence
void LightSequenceFailure(float deltaTime, bool restart_sequence = false);

void LightSequenceSuccess(float deltaTime,bool restart_sequence = false);

};

