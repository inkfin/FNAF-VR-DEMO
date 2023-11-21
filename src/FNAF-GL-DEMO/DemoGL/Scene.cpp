//
// Created by 11096 on 11/19/2023.
//

#include "Scene.h"

#include <memory>

#include <LoadTexture.h>
#include <InitShader.h>
#include <Shader.h>

#include "Game/GameScene.h"
#include "Window/DrawGui.h"

#include "Camera.h"

void Scene::Display(GLFWwindow* window)
{
    // Clear the screen to the color previously specified in the glClearColor(...) call.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update Uniform Buffer Object
    SceneData.P = glm::perspective(glm::pi<float>() / 4.0f, GlfwWindow::Aspect, 0.1f, 1000.0f);
    SceneData.V = camera->GetViewMatrix();
    SceneData.PV = SceneData.P * SceneData.V;

    // Render global objects
    GameScene::Render();

    DrawGui::Display(window);

    // Swap front and back buffers
    glfwSwapBuffers(window);
}

void Scene::Init()
{
    camera = std::make_unique<Camera>();
    DrawGui::Init();
    GameScene::Init();
}

void Scene::Idle()
{
    GameScene::Idle();
}
