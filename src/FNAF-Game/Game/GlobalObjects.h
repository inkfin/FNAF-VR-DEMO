//
// Created by 11096 on 11/19/2023.
//
#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Objects/Pawn.h>
#include <Objects/StaticMesh.h>
#include <Window/GlfwWindow.h>

#include "Shader.h"
#include "CameraInterface.h"

namespace Scene {

static const std::string map_name = "assets/Map2/Scene.gltf";

static const std::string ShaderDir = "shaders/";

// This structure mirrors the uniform block declared in the shader
struct SceneUniforms {
    glm::mat4 P = glm::perspective(glm::pi<float>() / 4.0f, GlfwWindow::Aspect, 0.1f, 1000.0f);
    glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 PV {}; // camera projection * view matrix
    glm::vec4 eye_w = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f); // world-space eye position
};
inline SceneUniforms SceneData;

struct LightUniforms {
    glm::vec4 La = glm::vec4(0.2f, 0.2f, 0.25f, 1.0f); // ambient light color
    glm::vec4 Ld = glm::vec4(0.5f, 0.5f, 0.25f, 1.0f); // diffuse light color
    glm::vec4 Ls = glm::vec4(0.3f); // specular light color
    glm::vec4 light_w = glm::vec4(0.0f, 2.2, 1.0f, 1.0f); // world-space light position
};
inline LightUniforms LightData;

struct MaterialUniforms {
    glm::vec4 ka = glm::vec4(0.312f, 0.261f, 0.139, 1.0f); // ambient material color
    glm::vec4 kd = glm::vec4(0.861f, 0.663f, 0.518f, 1.0f); // diffuse material color
    glm::vec4 ks = glm::vec4(1.0f); // specular material color
    float shininess = 20.0f; // specular exponent
};
inline MaterialUniforms MaterialData;

struct ControllerState {
    glm::vec2 trackpad_left = glm::vec2(0.f, 0.f);
    glm::vec2 trackpad_right {};

    bool trackpadClick_left = false;
    bool trackpadClick_right = false;

    bool triggerClick_left = false;
    bool triggerClick_right = false;

    bool squeezeClick_left = false;
    bool squeezeClick_right = false;

    bool menuClick_left = false;
    bool menuClick_right = false;

    glm::mat4 pose_left = glm::mat4(1.0f);
    glm::mat4 pose_right = glm::mat4(1.0f);
};

inline ControllerState gControllerState;

static int model_opt = 0;

namespace UboBinding {
    // These values come from the binding value specified in the shader block layout
    //    static const int scene = 0;
    static const int light = 1;
    static const int material = 2;
}

// IDs for the buffer objects holding the uniform block data
inline GLuint scene_ubo = -1;
inline GLuint light_ubo = -1;
inline GLuint material_ubo = -1;

// Some frame settings
inline bool bCaptureGui;
inline bool bRecordingBuffer;
inline bool bClearDefaultFb;

static const glm::vec3 map_position = glm::vec3(-1.f, -5.f, -30.7703f);
static const glm::vec3 map_rotation = glm::vec3(-90.f, 0.f, 0.f);
static const glm::vec3 freddy_position = glm::vec3(-1.78686f, 3.0f, -30.7703f);
static const glm::vec3 bunny_position = glm::vec3(-1.78686f, -5.f, -20.7703f);

static const std::string freddy_model = "assets/Characters/freddy/freddy_ill_walk.gltf";
static const std::string bunny_model = "assets/Characters/bunny/bunny_crawl.gltf";

inline Pawn gFreddy;
inline Pawn gBunny;

inline std::shared_ptr<StaticMesh> gMapMesh;

inline std::unique_ptr<ICameraInterface> camera;

// TODO: remove this later
inline glm::mat4 translation = glm::identity<glm::mat4>();
}
