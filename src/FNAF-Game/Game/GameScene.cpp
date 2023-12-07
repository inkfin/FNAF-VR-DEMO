//// Created by 11096 on 11/19/2023.
//

#include <GL/glew.h>
#include <InitShader.h>
#include <Shader.h>

#include "GameScene.h"
#include "GlobalObjects.h"
#include "Game.h"
#include "Objects/LightManager.h"
#include "Objects/TitleMesh.h"

using namespace Scene;

void GameScene::ModelInit()
{
    gStartMesh = std::make_unique<TitleMesh>();
    gStartMesh->LoadMesh(start_title_name);

    gEndMesh = std::make_unique<TitleMesh>();
    gEndMesh->LoadMesh(end_title_name);

    gWinMesh = std::make_unique<TitleMesh>();
    gWinMesh->LoadMesh(win_title_name);

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
    LightManager::InitLight();

    ModelInit();

    // DebugDraw::Init();

    // glGetIntegerv(GL_VIEWPORT, &SceneData.Viewport[0]);
    // Camera::Update();
    // Camera::UpdateP();
    // DrawGui::InitVr();

    JsonConfig::LoadConfig(R"(Configs\vr_ini_config.json)");
    JsonConfig::LoadGameLoopConfig(R"(Configs\game_logic_config.json)");
}

void GameScene::Render()
{
    Shader* pShader;

    pShader = TitleMesh::sShader();
    pShader->UseProgram();

    pShader->setUniform("PV", SceneData.PV);

    if (!Scene::is_game_started) {
        pShader->setUniform("M", glm::translate(glm::vec3(-6.5f, 2.f, -6.5f)) * glm::rotate(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(0.03f)));
        pShader->setUniform("color", glm::vec4(0.f, 1.f, 0.f, 1.f));
        gStartMesh->Render();
    }
    else if (Scene::is_game_over) {
        if (Scene::game_result) {
            pShader->setUniform("M", glm::translate(glm::vec3(-1.8f, 0.f, -6.5f)) * glm::rotate(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(0.005f)));
            pShader->setUniform("color", glm::vec4(0.f, 1.f, 0.f, 1.f));
            gWinMesh->Render();
        }
        else {
            if (Scene::EndTitleShow) {
                pShader->setUniform("M", glm::translate(glm::vec3(-1.8f, 0.f, -1.f)) * glm::rotate(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(0.005f)));
                pShader->setUniform("color", glm::vec4(1.f, 0.f, 0.f, 1.f));
                gEndMesh->Render();
            }
        }
    }

    // render Scene
    pShader = StaticMesh::sShader();
    pShader->UseProgram();

    // light
    LightManager::SetLightUniforms(pShader);

    pShader->setUniform("PV", SceneData.PV);
    pShader->setUniform("eye_w", SceneData.eye_w);

    pShader->setUniform("M", gMapMesh->GetModelMatrix());
    pShader->setUniform("Mode", 0);
    gMapMesh->Render();

    // render Anime Mesh
    pShader = SkinnedMesh::sShader();
    pShader->UseProgram();

    // light
    LightManager::SetLightUniforms(pShader);

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

    static float mesh_start_time;

    if (!Scene::is_game_started) mesh_start_time = 0.0f;
    if (Scene::is_game_started && mesh_start_time == 0.0f) {
        mesh_start_time = time_sec;
    }
    float mesh_time_sec = time_sec - mesh_start_time;

    //    gMapMesh->Update(time_sec);
    if (!Scene::gFreddy.mStatus.active) {
        gFreddy.mMesh->Update(0);
    }
    else {
        gFreddy.mMesh->Update(mesh_time_sec * Scene::rate);
    }

    if (!Scene::gBunny.mStatus.active) {
        gBunny.mMesh->Update(0);
    }
    else {
        gBunny.mMesh->Update(mesh_time_sec);
    }

    //StaticMesh::sShader()->setUniform("time", time_sec);

    // Pawn
}
