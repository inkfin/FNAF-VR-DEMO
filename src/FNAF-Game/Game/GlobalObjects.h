#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Objects/Pawn.h>
#include <Objects/StaticMesh.h>
#include <Window/GlfwWindow.h>

#include "Shader.h"
#include "CameraInterface.h"
#include "Objects/TitleMesh.h"

namespace Scene {

static const std::string map_name = "assets/Map2/Scene.gltf";
static const std::string start_title_name = "assets/Title/Start.gltf";
static const std::string end_title_name = "assets/Title/End.gltf";
static const std::string win_title_name = "assets/Title/Win.gltf";

static const std::string ShaderDir = "shaders/";

// This structure mirrors the uniform block declared in the shader
struct SceneUniforms {
    glm::mat4 P = glm::perspective(glm::pi<float>() / 4.0f, GlfwWindow::Aspect, 0.1f, 1000.0f);
    glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 PV {}; // camera projection * view matrix
    glm::vec4 eye_w = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f); // world-space eye position
};
inline SceneUniforms SceneData;


struct MaterialUniforms {
    glm::vec4 ka = glm::vec4(0.312f, 0.261f, 0.139, 1.0f); // ambient material color
    glm::vec4 kd = glm::vec4(0.861f, 0.663f, 0.518f, 1.0f); // diffuse material color
    glm::vec4 ks = glm::vec4(1.0f); // specular material color
    float shininess = 40.0f; // specular exponent
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

inline std::unique_ptr<TitleMesh> gStartMesh;
inline std::unique_ptr<TitleMesh> gEndMesh;
inline std::unique_ptr<TitleMesh> gWinMesh;

// TODO: remove this later
inline glm::mat4 translation = glm::identity<glm::mat4>();

inline int game_mode = 0;

inline bool is_game_started = false;
inline bool is_game_over = false;

inline float freddy_ini_speed = 0.6f;
inline float rate = 1.f;

struct gameLoopConfig {
    int game_time;
    float freddy_death_distance;
    int bunny_show_time;
    int bunny_show_rate;
    int bunny_react_time;
    float speed_increase_rate;
};
inline gameLoopConfig game_loop_config;
// 0 for lose, 1 for win
inline bool game_result;
inline bool key_flash_light;

inline bool freddy_stop_flag = 0;
inline bool bunny_show_flag;
inline int bunny_hold_count;
inline int bunny_death_count = 0;
constexpr int bunny_clear_time = 60; // frames
inline int game_time_count;
inline int EndTitleShow;

// GL speed
inline float dark_a = 0.65f;
inline float bright_a = 0.3f;

// VR Speed
inline float dark_a_VR = 0.65f;
inline float bright_a_VR = 0.3f;
}
