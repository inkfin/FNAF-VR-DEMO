//// Created by 11096 on 11/19/2023.
//

#include <GL/glew.h>
#include <InitShader.h>
#include <Shader.h>

#include "GameScene.h"
#include "GlobalObjects.h"
#include "Game.h"

using namespace Scene;

void GameScene::ModelInit()
{
    // initialize map
    gMapMesh = std::make_shared<StaticMesh>();
    gMapMesh->LoadMesh(map_name);

    gMapMesh->mTranslation = map_position;
    gMapMesh->mScale = glm::vec3(1.f, 1.f, 1.f);
    gMapMesh->mRotation = map_rotation;

    // initialize freddy
    gFreddy.mMesh = std::make_unique<SkinnedMesh>();
    gFreddy.mMesh->LoadMesh(freddy_model);

    gFreddy.mMesh->mTranslation = freddy_position;
    gFreddy.mMesh->mRotation = glm::vec3(180.f, 0.f, 0.f);
    gFreddy.mMesh->mScale = glm::vec3(0.5f, -0.5f, -0.5f);

    // initialize bunny
    gBunny.mMesh = std::make_unique<SkinnedMesh>();
    gBunny.mMesh->LoadMesh(bunny_model);

    gBunny.mMesh->mTranslation = bunny_position;
    gBunny.mMesh->mRotation = glm::vec3(-5.f, -100.f, -5.f);
    gBunny.mMesh->mScale = glm::vec3(0.016f, -0.016f, -0.016f);
}

void GameScene::Init()
{
    SetShaderDir(ShaderDir);

#pragma region OpenGL initial state
    // glClearColor(SceneData.clear_color.r, SceneData.clear_color.g, SceneData.clear_color.b, SceneData.clear_color.a);

    glEnable(GL_DEPTH_TEST);

    // Enable gl_PointCoord in shader
    glEnable(GL_POINT_SPRITE);
    // Allow setting point size in fragment shader
    glEnable(GL_PROGRAM_POINT_SIZE);
/*
    if (enable_msaa)
    {
        glEnable(GL_MULTISAMPLE);
    }
    else
    {
        glDisable(GL_MULTISAMPLE);
    }
*/
#pragma endregion

    bClearDefaultFb = false;

    // init light
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

    ModelInit();

    // DebugDraw::Init();

    // glGetIntegerv(GL_VIEWPORT, &SceneData.Viewport[0]);
    // Camera::Update();
    // Camera::UpdateP();
    // DrawGui::InitVr();

    JsonConfig::LoadConfig(R"(Configs\vr_ini_config.json)");
}

void GameScene::Render()
{
    Shader* pShader;

    // render Scene
    pShader = StaticMesh::sShader();
    pShader->UseProgram();

    // light
    pShader->setUniform("use_flash_light", use_flash_light);
    pShader->setUniform("shininess", 40.f);

    pShader->setUniform("dirLight.position", dirLightData.position);
    pShader->setUniform("dirLight.La", dirLightData.La);
    pShader->setUniform("dirLight.Ld", dirLightData.Ld);
    pShader->setUniform("dirLight.Ls", dirLightData.Ls);

    pShader->setUniform("spotLight.direction", spotLightData.direction);
    pShader->setUniform("spotLight.cutoff", spotLightData.cutOff);
    pShader->setUniform("spotLight.position", spotLightData.position);
    pShader->setUniform("spotLight.La", spotLightData.La);
    pShader->setUniform("spotLight.Ld", spotLightData.Ld);
    pShader->setUniform("spotLight.Ls", spotLightData.Ls);
    pShader->setUniform("spotLight.constant", spotLightData.constant);
    pShader->setUniform("spotLight.linear", spotLightData.linear);
    pShader->setUniform("spotLight.quadratic", spotLightData.quadratic);

    for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
        pShader->setUniform("pointLights[" + std::to_string(i) + "].position", pointLightData[i].position);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].La", pointLightData[i].La);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].Ld", pointLightData[i].Ld);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].Ls", pointLightData[i].Ls);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].constant", pointLightData[i].constant);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].linear", pointLightData[i].linear);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].quadratic", pointLightData[i].quadratic);
    }

    pShader->setUniform("PV", SceneData.PV);
    pShader->setUniform("eye_w", SceneData.eye_w);

    pShader->setUniform("M", gMapMesh->GetModelMatrix());
    pShader->setUniform("Mode", 0);
    gMapMesh->Render();

    // render Anime Mesh
    pShader = SkinnedMesh::sShader();
    pShader->UseProgram();

    // light
    pShader->setUniform("use_flash_light", use_flash_light);
    pShader->setUniform("shininess", 40.f);

    pShader->setUniform("dirLight.position", dirLightData.position);
    pShader->setUniform("dirLight.La", dirLightData.La);
    pShader->setUniform("dirLight.Ld", dirLightData.Ld);
    pShader->setUniform("dirLight.Ls", dirLightData.Ls);

    pShader->setUniform("spotLight.direction", spotLightData.direction);
    pShader->setUniform("spotLight.cutoff", spotLightData.cutOff);
    pShader->setUniform("spotLight.position", spotLightData.position);
    pShader->setUniform("spotLight.La", spotLightData.La);
    pShader->setUniform("spotLight.Ld", spotLightData.Ld);
    pShader->setUniform("spotLight.Ls", spotLightData.Ls);
    pShader->setUniform("spotLight.constant", spotLightData.constant);
    pShader->setUniform("spotLight.linear", spotLightData.linear);
    pShader->setUniform("spotLight.quadratic", spotLightData.quadratic);

    for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
        pShader->setUniform("pointLights[" + std::to_string(i) + "].position", pointLightData[0].position);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].La", pointLightData[0].La);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].Ld", pointLightData[0].Ld);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].Ls", pointLightData[0].Ls);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].constant", pointLightData[0].constant);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].linear", pointLightData[0].linear);
        pShader->setUniform("pointLights[" + std::to_string(i) + "].quadratic", pointLightData[0].quadratic);
    }

    pShader->setUniform("PV", SceneData.PV);
    pShader->setUniform("eye_w", SceneData.eye_w);

    // Freddy
    pShader->setUniform("M", gFreddy.GetModelMatrix());
    pShader->setUniform("Mode", 1);
    gFreddy.mMesh->Render();

    // Bunny
    pShader->setUniform("M", gBunny.GetModelMatrix());
    pShader->setUniform("Mode", 1);
    gBunny.mMesh->Render();

    //    DebugDraw::DrawAxis();
}

void GameScene::RenderVR()
{
}

void GameScene::Idle()
{
    constexpr float fixed_time_step = 1.0f / 60.0f;
    static float time_passed = 0.0f; // update fixed step every 1/60 sec

    static float prev_time_sec = 0.0f;
    float time_sec = static_cast<float>(glfwGetTime());
    const float dt = time_sec - prev_time_sec;

    Game::UpdateDynamicStep(dt);

    int max_iter = 2;
    while (time_passed >= fixed_time_step && --max_iter) {
        Game::UpdateFixedStep();
        time_passed -= fixed_time_step;
    }

    prev_time_sec = time_sec;
    time_passed += dt;

    //    gMapMesh->Update(time_sec);
    gFreddy.mMesh->Update(time_sec);
    gBunny.mMesh->Update(time_sec);

    StaticMesh::sShader()->setUniform("time", time_sec);

    // Pawn
}
