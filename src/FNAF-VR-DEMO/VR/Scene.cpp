#include "Scene.h"
#include <iostream>

#include "Camera.h"
#include "Game/GameScene.h"
#include "Window/DrawGui.h"

void Scene::Display(GLFWwindow* window)
{
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);


    if (bClearDefaultFb) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    GameScene::Render();

    DrawGui::Display(window);

    // Swap front and back buffers
    glfwSwapBuffers(window);
}

void Scene::DisplayVr(const glm::mat4& P, const glm::mat4& V)
{
    // No clear in this function

    SceneData.P = P;
    
    auto* pCamera = dynamic_cast<Camera*>(camera.get());
    if (pCamera) {
        pCamera->Update(V);
    }
    SceneData.V = pCamera->GetViewMatrix();

    SceneData.PV = SceneData.P * SceneData.V;

    GameScene::Render();
    GameScene::RenderVR();

    // No swap buffers in this function
}

void Scene::Init()
{
    camera = std::make_unique<Camera>();
    DrawGui::Init();
    GameScene::Init();
}

// first call Idle(), then call Display()
void Scene::Idle()
{
    GameScene::Idle();
}

