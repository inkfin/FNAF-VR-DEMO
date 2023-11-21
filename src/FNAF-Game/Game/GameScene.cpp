//
// Created by 11096 on 11/19/2023.
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
    gMapMesh = std::make_shared<SkinnedMesh>();
    gMapMesh->LoadMesh(map_name);

    gMapMesh->mTranslation = map_position;
    gMapMesh->mScale= glm::vec3(1.f, 1.f, 1.f);
    gMapMesh->mRotation = map_rotation;

    // initialize freddy
    gFreddy.mMesh = std::make_unique<AnimeMesh>();
    gFreddy.mMesh->LoadMesh(freddy_model);

    gFreddy.mMesh->mTranslation = freddy_position;
    gFreddy.mMesh->mRotation = glm::vec3(180.f, 0.f, 0.f);
    gFreddy.mMesh->mScale = glm::vec3(0.5f, -0.5f, -0.5f);

    // initialize bunny
    gBunny.mMesh = std::make_unique<AnimeMesh>();
    gBunny.mMesh->LoadMesh(bunny_model);

    gBunny.mMesh->mTranslation = bunny_position;
    gBunny.mMesh->mRotation = glm::vec3(-5.f, -100.f, -5.f);
    gBunny.mMesh->mScale = glm::vec3(0.016f, -0.016f, -0.016f);
}

void GameScene::ShaderInit() {

    skinned_shader_program = std::make_shared<Shader>(skinned_vertex_shader.c_str(), skinned_fragment_shader.c_str());
    skinned_shader_program->Init();

}

void GameScene::Render() {

    skinned_shader_program->UseProgram();

    skinned_shader_program->setUniform("PV", SceneData.PV);
    skinned_shader_program->setUniform("M", gMapMesh->GetModelMatrix());
    skinned_shader_program->setUniform("Mode", 0);
    gMapMesh->Render();

    // Freddy
    skinned_shader_program->setUniform("PV", SceneData.PV);
    skinned_shader_program->setUniform("M", gFreddy.GetModelMatrix());
    skinned_shader_program->setUniform("Mode", 1);
    gFreddy.mMesh->Render();

    // Bunny
    skinned_shader_program->setUniform("PV", SceneData.PV);
    skinned_shader_program->setUniform("M", gBunny.GetModelMatrix());
    skinned_shader_program->setUniform("Mode", 1);
    gBunny.mMesh->Render();

//    DebugDraw::DrawAxis();
}

void GameScene::RenderVR() {

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

    gMapMesh->Update(time_sec);
    gFreddy.mMesh->Update(time_sec);
    gBunny.mMesh->Update(time_sec);

    skinned_shader_program->setUniform("time", time_sec);

    // Pawn
}


void GameScene::Init()
{
    SetShaderDir(ShaderDir);
    //SetMeshDir(MeshDir);
    //SetTextureDir(TextureDir);

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

    // Create and initialize uniform buffers
    glGenBuffers(1, &scene_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, scene_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneUniforms), nullptr, GL_STREAM_DRAW); // Allocate memory for the buffer, but don't copy (since pointer is null).
    glBindBufferBase(GL_UNIFORM_BUFFER, UboBinding::scene, scene_ubo); // Associate this uniform buffer with the uniform block in the shader that has the same binding.

    glGenBuffers(1, &light_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightUniforms), &LightData, GL_STREAM_DRAW); // Allocate memory for the buffer, but don't copy (since pointer is null).
    glBindBufferBase(GL_UNIFORM_BUFFER, UboBinding::light, light_ubo); // Associate this uniform buffer with the uniform block in the shader that has the same binding.

    glGenBuffers(1, &material_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, material_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUniforms), &MaterialData, GL_STREAM_DRAW); // Allocate memory for the buffer, but don't copy (since pointer is null).
    glBindBufferBase(GL_UNIFORM_BUFFER, UboBinding::material, material_ubo); // Associate this uniform buffer with the uniform block in the shader that has the same binding.

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    ModelInit();

    ShaderInit();

//    DebugDraw::Init();

    // glGetIntegerv(GL_VIEWPORT, &SceneData.Viewport[0]);
    // Camera::Update();
    // Camera::UpdateP();
    // DrawGui::InitVr();
}


namespace GameScene {

}
