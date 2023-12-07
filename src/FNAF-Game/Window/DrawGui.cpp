#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "DrawGui.h"
#include <Game/GlobalObjects.h>
#include "Objects/LightManager.h"
#include "Game/JsonConfig.h"

bool DrawGui::HideGui = false;

namespace DrawGui {

void Init()
{
    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(GlfwWindow::window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Display(GLFWwindow* window)
{
    if (HideGui == true)
        return;
    // Begin ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool show_debug_window = false;
    static bool show_capture_options = false;
    static bool show_imgui_test = false;
    static bool show_spotlight_manager = false;
    static bool show_pointlight0_manager = false;
    static bool show_pointlight1_manager = false;
    static bool show_pointlight2_manager = false;
    static bool show_pointlight3_manager = false;

#pragma region menubar

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Uniforms")) {

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Blit")) {

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Capture")) {
            if (ImGui::MenuItem("Show capture options", 0, show_capture_options)) {
                show_capture_options = !show_capture_options;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug")) {
            if (ImGui::MenuItem("Show/Hide GUI", "F1", HideGui)) {
                HideGui = !HideGui;
            }
            if (ImGui::MenuItem("Debug Menu", 0, show_debug_window)) {
                show_debug_window = !show_debug_window;
            }

            if (ImGui::MenuItem("ImGui Test Window", 0, show_imgui_test)) {
                show_imgui_test = !show_imgui_test;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Light")) {
            if (ImGui::MenuItem("Spot Light", 0, show_spotlight_manager)) {
                show_spotlight_manager = !show_spotlight_manager;
            }
            if (ImGui::MenuItem("Point Light 0", 0, show_pointlight0_manager)) {
                show_pointlight0_manager = !show_pointlight0_manager;
            }
            if (ImGui::MenuItem("Point Light 1", 0, show_pointlight1_manager)) {
                show_pointlight1_manager = !show_pointlight1_manager;
            }
            if (ImGui::MenuItem("Point Light 2", 0, show_pointlight2_manager)) {
                show_pointlight2_manager = !show_pointlight2_manager;
            }
            if (ImGui::MenuItem("Point Light 3", 0, show_pointlight3_manager)) {
                show_pointlight3_manager = !show_pointlight3_manager;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
#pragma endregion

    if (show_debug_window) {
        {
            ImGui::Begin("Debug window", &show_debug_window);
            if (ImGui::Button("Quit")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            auto cam_pos = Scene::camera->GetLocation();
            ImGui::Text("Camera position: (%.2f, %.2f, %.2f)", cam_pos.x, cam_pos.y, cam_pos.z);
            auto cam_forward = Scene::camera->GetForward();
            ImGui::Text("Camera position: (%.2f, %.2f, %.2f)", cam_forward.x, cam_forward.y, cam_forward.z);
            ImGui::Text("Trackpad left, (%.2f, %.2f)", Scene::gControllerState.trackpad_left.x, Scene::gControllerState.trackpad_left.y);

            ImGui::Text("Till Bunny kill you: <%.2f>", (float)Scene::bunny_death_count / Scene::game_loop_config.bunny_react_time);

//            static int mode = 0;
//            ImGui::Text("AnimeMesh Mode");
//            ImGui::RadioButton("Rest pose", &mode, 0);
//            ImGui::RadioButton("LBS", &mode, 1);
//            ImGui::RadioButton("Debug", &mode, 2);
//
//            glUniform1i(SkinnedMesh::UniformLoc::Mode, mode);
            ImGui::Text("Rate: <%.2f>", Scene::rate);

            ImGui::End();
        }
    }

    if (show_spotlight_manager) {
        ImGui::Begin("====== Spot light ======", &show_spotlight_manager);
        ImGui::Checkbox("Use flash light", &LightManager::use_flash_light);
        ImGui::DragFloat3("direction", glm::value_ptr(LightManager::spotLightData.direction), 0.1f);
        ImGui::SliderFloat("cut off", &LightManager::spotLightData.cutOff, 0.0f, 1.0f);
        ImGui::DragFloat3("position", glm::value_ptr(LightManager::spotLightData.position), 0.1f);
        ImGui::ColorEdit3("ambient", glm::value_ptr(LightManager::spotLightData.La));
        ImGui::ColorEdit3("diffuse", glm::value_ptr(LightManager::spotLightData.Ld));
        ImGui::ColorEdit3("specular", glm::value_ptr(LightManager::spotLightData.Ls));
        ImGui::SliderFloat("constant", &LightManager::spotLightData.constant, 0.0f, 1.0f);
        ImGui::SliderFloat("linear", &LightManager::spotLightData.linear, 0.0f, 0.1f);
        ImGui::SliderFloat("quadratic", &LightManager::spotLightData.quadratic, 0.0f, 0.01f);
        ImGui::End();
    }

    if (show_pointlight0_manager) {
        ImGui::Begin("====== Point Light 0 ======", &show_pointlight0_manager);
        ImGui::Checkbox("Light on", &LightManager::pointLightData[0].isOn);
        ImGui::DragFloat3("position", glm::value_ptr(LightManager::pointLightData[0].position), 0.1f);
        ImGui::ColorEdit3("ambient", glm::value_ptr(LightManager::pointLightData[0].La));
        ImGui::ColorEdit3("diffuse", glm::value_ptr(LightManager::pointLightData[0].Ld));
        ImGui::ColorEdit3("specular", glm::value_ptr(LightManager::pointLightData[0].Ls));
        ImGui::SliderFloat("constant", &LightManager::pointLightData[0].constant, 0.0f, 1.0f);
        ImGui::SliderFloat("linear", &LightManager::pointLightData[0].linear, 0.0f, 0.1f);
        ImGui::SliderFloat("quadratic", &LightManager::pointLightData[0].quadratic, 0.0f, 0.01f);
        ImGui::End();
    }

    if (show_pointlight1_manager) {
        ImGui::Begin("====== Point Light 1 ======", &show_pointlight1_manager);
        ImGui::Checkbox("Light on", &LightManager::pointLightData[1].isOn);
        ImGui::DragFloat3("position", glm::value_ptr(LightManager::pointLightData[1].position), 0.1f);
        ImGui::ColorEdit3("ambient", glm::value_ptr(LightManager::pointLightData[1].La));
        ImGui::ColorEdit3("diffuse", glm::value_ptr(LightManager::pointLightData[1].Ld));
        ImGui::ColorEdit3("specular", glm::value_ptr(LightManager::pointLightData[1].Ls));
        ImGui::SliderFloat("constant", &LightManager::pointLightData[1].constant, 0.0f, 1.0f);
        ImGui::SliderFloat("linear", &LightManager::pointLightData[1].linear, 0.0f, 0.1f);
        ImGui::SliderFloat("quadratic", &LightManager::pointLightData[1].quadratic, 0.0f, 0.01f);
        ImGui::End();
    }

    if (show_pointlight2_manager) {
        ImGui::Begin("====== Point Light 2 ======", &show_pointlight2_manager);
        ImGui::Checkbox("Light on", &LightManager::pointLightData[2].isOn);
        ImGui::DragFloat3("position", glm::value_ptr(LightManager::pointLightData[2].position), 0.1f);
        ImGui::ColorEdit3("ambient", glm::value_ptr(LightManager::pointLightData[2].La));
        ImGui::ColorEdit3("diffuse", glm::value_ptr(LightManager::pointLightData[2].Ld));
        ImGui::ColorEdit3("specular", glm::value_ptr(LightManager::pointLightData[2].Ls));
        ImGui::SliderFloat("constant", &LightManager::pointLightData[2].constant, 0.0f, 1.0f);
        ImGui::SliderFloat("linear", &LightManager::pointLightData[2].linear, 0.0f, 0.1f);
        ImGui::SliderFloat("quadratic", &LightManager::pointLightData[2].quadratic, 0.0f, 0.01f);
        ImGui::End();
    }

    if (show_pointlight3_manager) {
        ImGui::Begin("====== Point Light 3 ======", &show_pointlight3_manager);
        ImGui::Checkbox("Light on", &LightManager::pointLightData[3].isOn);
        ImGui::DragFloat3("position", glm::value_ptr(LightManager::pointLightData[3].position), 0.1f);
        ImGui::ColorEdit3("ambient", glm::value_ptr(LightManager::pointLightData[3].La));
        ImGui::ColorEdit3("diffuse", glm::value_ptr(LightManager::pointLightData[3].Ld));
        ImGui::ColorEdit3("specular", glm::value_ptr(LightManager::pointLightData[3].Ls));
        ImGui::SliderFloat("constant", &LightManager::pointLightData[3].constant, 0.0f, 1.0f);
        ImGui::SliderFloat("linear", &LightManager::pointLightData[3].linear, 0.0f, 0.1f);
        ImGui::SliderFloat("quadratic", &LightManager::pointLightData[3].quadratic, 0.0f, 0.01f);
        ImGui::End();
    }

    if (show_imgui_test) {
        ImGui::ShowDemoWindow(&show_imgui_test);
    }

    // End ImGui Frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
};