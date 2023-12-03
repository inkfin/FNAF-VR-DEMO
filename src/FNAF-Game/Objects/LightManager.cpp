//
// Created by 11096 on 12/2/2023.
//

#include <random>
#include "Game/GlobalObjects.h"
#include "Shader.h"

#include "LightManager.h"

void LightManager::InitLight()
{
    spotLightData.direction = glm::vec3(0.0f, -0.2f, -1.0f);
    spotLightData.cutOff = 0.7f;

    spotLightData.position = glm::vec3(0.0f, 0.0f, -10.0f); // world-space light position
    spotLightData.La = glm::vec3(0.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    spotLightData.Ld = glm::vec3(255.0f, 255.0f, 255.0f) / 255.0f; // diffuse light color
    spotLightData.Ls = glm::vec3(255.0f, 255.0f, 255.0f) / 255.0f; // specular light color
    spotLightData.constant = 1.0f;
    spotLightData.linear = 0.022f;
    spotLightData.quadratic = 0.0003f;

    LightOn();
}

void LightManager::SetLightUniforms(const Shader* pShader)
{
    pShader->setUniform("shininess", 40.f);

    // directional light
    pShader->setUniform("dirLight.position", dirLightData.position);
    pShader->setUniform("dirLight.La", dirLightData.La);
    pShader->setUniform("dirLight.Ld", dirLightData.Ld);
    pShader->setUniform("dirLight.Ls", dirLightData.Ls);

    // spotlight
    pShader->setUniform("spotLight.direction", spotLightData.direction);
    pShader->setUniform("spotLight.cutoff", spotLightData.cutOff);
    pShader->setUniform("spotLight.position", spotLightData.position);
    pShader->setUniform("spotLight.La", spotLightData.La);
    pShader->setUniform("spotLight.Ld", spotLightData.Ld);
    pShader->setUniform("spotLight.Ls", spotLightData.Ls);
    pShader->setUniform("spotLight.constant", spotLightData.constant);
    pShader->setUniform("spotLight.linear", spotLightData.linear);
    pShader->setUniform("spotLight.quadratic", spotLightData.quadratic);
    pShader->setUniform("spotLight.isOn", use_flash_light);

    // point lights
    for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
        pShader->setUniform("pointLights[" + std::to_string(i) + "].position", pointLightData[i].position);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].La", pointLightData[i].La);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].Ld", pointLightData[i].Ld);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].Ls", pointLightData[i].Ls);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].constant", pointLightData[i].constant);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].linear", pointLightData[i].linear);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].quadratic", pointLightData[i].quadratic);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].isOn", pointLightData[i].isOn);
    }
}

// Light control
void LightManager::LightOn()
{
    pointLightData[0].position = glm::vec3(12.0f, 9.2f, -12.0f); // world-space light position
    pointLightData[0].La = glm::vec3(0.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[0].Ld = glm::vec3(230.0f, 107.0f, 140.0f) / 255.0f; // diffuse light color
    pointLightData[0].Ls = glm::vec3(203.0f, 203.0f, 203.0f) / 255.0f; // specular light color
    pointLightData[0].constant = 0.174f;
    pointLightData[0].linear = 0.067f;
    pointLightData[0].quadratic = 0.010f;
    pointLightData[0].isOn = true;

    pointLightData[1].position = glm::vec3(-22.3f, 13.0f, -8.8f); // world-space light position
    pointLightData[1].La = glm::vec3(0.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[1].Ld = glm::vec3(219.0f, 134.0f, 255.0f) / 255.0f; // diffuse light color
    pointLightData[1].Ls = glm::vec3(9.0f, 9.0, 9.0f) / 255.0f; // specular light color
    pointLightData[1].constant = 0.118f;
    pointLightData[1].linear = 0.031f;
    pointLightData[1].quadratic = 0.005f;
    pointLightData[1].isOn = true;

    pointLightData[2].position = glm::vec3(14.2f, -5.3f, 7.6f); // world-space light position
    pointLightData[2].La = glm::vec3(0.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[2].Ld = glm::vec3(204.0f, 85.0f, 255.0f) / 255.0f; // diffuse light color
    pointLightData[2].Ls = glm::vec3(106.0f) / 255.0f; // specular light color
    pointLightData[2].constant = 0.000f;
    pointLightData[2].linear = 0.064f;
    pointLightData[2].quadratic = 0.010f;
    pointLightData[2].isOn = true;

    pointLightData[3].position = glm::vec3(-16.2f, -4.2f, 5.1f); // world-space light position
    pointLightData[3].La = glm::vec3(0.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[3].Ld = glm::vec3(121.0f, 200.0f, 255.0f) / 255.0f; // diffuse light color
    pointLightData[3].Ls = glm::vec3(151.0f, 151.0, 151.0f) / 255.0f; // specular light color
    pointLightData[3].constant = 0.220f;
    pointLightData[3].linear = 0.094f;
    pointLightData[3].quadratic = 0.005f;
    pointLightData[3].isOn = true;
}

void LightManager::LightOff()
{
    pointLightData[0].isOn = false;
    pointLightData[1].isOn = false;
    pointLightData[2].isOn = false;
    pointLightData[3].isOn = false;
}

void LightManager::LightFailure()
{
    // Red Light
    pointLightData[0].La = glm::vec3(80.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[0].Ld = glm::vec3(230.0f, 0.0f, 0.0f) / 255.0f; // diffuse light color
    pointLightData[0].isOn = true;

    pointLightData[1].Ld = glm::vec3(219.0f, 0.0f, 0.0f) / 255.0f; // diffuse light color
    pointLightData[1].isOn = true;

    pointLightData[2].Ld = glm::vec3(204.0f, 0.0f, 0.0f) / 255.0f; // diffuse light color
    pointLightData[2].isOn = true;

    pointLightData[3].Ld = glm::vec3(121.0f, 0.0f, 0.0f) / 255.0f; // diffuse light color
    pointLightData[3].isOn = true;
}

void LightManager::LightSuccess() {
    // White Light
    pointLightData[0].La = glm::vec3(80.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[0].Ld = glm::vec3(230.0f) / 255.0f; // diffuse light color
    pointLightData[0].isOn = true;

    pointLightData[1].Ld = glm::vec3(219.0f) / 255.0f; // diffuse light color
    pointLightData[1].isOn = true;

    pointLightData[2].Ld = glm::vec3(204.0f) / 255.0f; // diffuse light color
    pointLightData[2].isOn = true;

    pointLightData[3].Ld = glm::vec3(121.0f) / 255.0f; // diffuse light color
    pointLightData[3].isOn = true;
}

std::vector<glm::vec3> light_colors {
    { 230.0f, 107.0f, 253.0f },
    { 216.0f, 181.0f, 216.0f },
    { 170.0f, 255.0f, 140.0f },
    { 111.0f, 255.0f, 238.0f },
};

void LightManager::LightMarquee(float deltaTime, float interval)
{
    static float time_passed = 0.0f;
    static int light_index = 0;

    static auto lerp = [](glm::vec3 a, glm::vec3 b, float t) {
        return a + (b - a) * t;
    };

    time_passed += deltaTime;
    if (time_passed > interval) {
        time_passed = 0.0f;
        light_index = (light_index + 1) % POINT_LIGHT_COUNT;
    }

    for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
        PointLightUniforms& light = pointLightData[(i + light_index) % POINT_LIGHT_COUNT];
        light.isOn = true;
        light.Ld = lerp(light.Ld, (glm::vec3(light_colors[i]) / 255.0f), time_passed / interval);
    }
}

void LightManager::LightGlitter(float deltaTime, float on_interval_max, float on_interval_min, float off_interval_max, float off_interval_min)
{
    static float time_passed = 0.0f;
    static float interval = 0.0f;
    static bool isOn = false;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> distrib(0, 1);

    if (interval == 0.0f) {
        if (isOn)
            interval = on_interval_min + distrib(gen) * (on_interval_max - on_interval_min);
        else
            interval = off_interval_min + distrib(gen) * (off_interval_max - off_interval_min);
    }

    time_passed += deltaTime;
    if (time_passed > interval) {
        time_passed = 0.0f;
        interval = 0.0f;

        isOn = !isOn;
    }

    for (auto& i : pointLightData) {
        i.isOn = isOn;
    }
}

void LightManager::LightSequenceFailure(float deltaTime, bool restart_sequence)
{
    static float time_passed = 0.0f;
    constexpr int sequence_length = 4;
    const float time_interval[sequence_length] = { 1.0f, 2.0f, 0.1f, 0.1f };

    if (restart_sequence) {
        time_passed = 0.0f;
    }

    time_passed += deltaTime;

    if (time_passed > time_interval[0] + time_interval[1] + time_interval[2]) {
        // do nothing
        return;
    } else if (time_passed > time_interval[0] + time_interval[1]) {
        // stage 3: turn on all lights (failure)
        LightManager::LightFailure();
        return;
    } else if (time_passed > time_interval[0]) {
        // stage 1: play light glitter effect
        LightManager::LightGlitter(deltaTime, 0.1f, 0.01f, 0.5f, 0.1f);
        return;
    } else {
        // stage 0: turn off all lights
        LightManager::LightOff();
        return;
    }
}

void LightManager::LightSequenceSuccess(float deltaTime, bool restart_sequence)
{
    static float time_passed = 0.0f;
    constexpr int sequence_length = 1;
    const float time_interval[sequence_length] = { 2.0f };

    if (restart_sequence) {
        time_passed = 0.0f;
    }

    time_passed += deltaTime;
    if (time_passed > time_interval[0]) {
        // stage 1: play light marquee effect
        LightMarquee(deltaTime, 0.5f);
        return;
    } else {
        // stage 0: turn on all lights (success)
        LightSuccess();
        return;
    }
}
