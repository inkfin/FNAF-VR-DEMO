//
// Created by 11096 on 12/2/2023.
//

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

    pointLightData[0].position = glm::vec3(12.0f, 9.2, -12.0f); // world-space light position
    pointLightData[0].La = glm::vec3(0.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[0].Ld = glm::vec3(230.0f, 107.0f, 140.0f) / 255.0f; // diffuse light color
    pointLightData[0].Ls = glm::vec3(203.0f, 203.0f, 203.0f) / 255.0f; // specular light color
    pointLightData[0].constant = 0.174f;
    pointLightData[0].linear = 0.067f;
    pointLightData[0].quadratic = 0.010f;

    pointLightData[1].position = glm::vec3(-22.3f, 13.0, -8.8f); // world-space light position
    pointLightData[1].La = glm::vec3(0.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[1].Ld = glm::vec3(219.0f, 134.0f, 255.0f) / 255.0f; // diffuse light color
    pointLightData[1].Ls = glm::vec3(9.0f, 9.0, 9.0f) / 255.0f; // specular light color
    pointLightData[1].constant = 0.118f;
    pointLightData[1].linear = 0.031f;
    pointLightData[1].quadratic = 0.005f;

    pointLightData[2].position = glm::vec3(-22.3f, 13.0, -8.8f); // world-space light position
    pointLightData[2].La = glm::vec3(0.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[2].Ld = glm::vec3(219.0f, 134.0f, 255.0f) / 255.0f; // diffuse light color
    pointLightData[2].Ls = glm::vec3(9.0f, 9.0, 9.0f) / 255.0f; // specular light color
    pointLightData[2].constant = 0.118f;
    pointLightData[2].linear = 0.031f;
    pointLightData[2].quadratic = 0.005f;

    pointLightData[3].position = glm::vec3(-22.3f, 13.0, -8.8f); // world-space light position
    pointLightData[3].La = glm::vec3(0.0f, 0.0f, 0.0f) / 255.0f; // ambient light color
    pointLightData[3].Ld = glm::vec3(219.0f, 134.0f, 255.0f) / 255.0f; // diffuse light color
    pointLightData[3].Ls = glm::vec3(9.0f, 9.0, 9.0f) / 255.0f; // specular light color
    pointLightData[3].constant = 0.118f;
    pointLightData[3].linear = 0.031f;
    pointLightData[3].quadratic = 0.005f;
}


void LightManager::SetLightUniforms(const Shader* pShader)
{
    pShader->setUniform("use_flash_light", use_flash_light);
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

    // point lights
    for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
        pShader->setUniform("pointLights[" + std::to_string(i) + "].position", pointLightData[i].position);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].La", pointLightData[i].La);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].Ld", pointLightData[i].Ld);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].Ls", pointLightData[i].Ls);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].constant", pointLightData[i].constant);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].linear", pointLightData[i].linear);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].quadratic", pointLightData[i].quadratic);
    }
}
