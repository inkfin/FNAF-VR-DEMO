#pragma once

#include "GlfwWindow.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "xr_common/xr_linear.h"
#include "imgui.h"
#include <iostream>
#include "Shader.h"

#include "Pawn.h"
#include "SkinnedMesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MeshBase;

namespace Camera {

void UpdateP();
void UpdateV(glm::vec3 delta_pos);

}

namespace Scene {
// This structure mirrors the uniform block declared in the shader
struct SceneUniforms {
    glm::mat4 P = glm::perspective(glm::pi<float>() / 4.0f, GlfwWindow::Aspect, 0.1f, 100.0f);
    glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 PV; // camera projection * view matrix
    glm::vec4 eye_w = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f); // world-space eye position
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.f, 3.0f));
    //float rotation_angle = 135.f;
    //glm::vec3 map_position = glm::vec3(4.29809f, -5.f, -29.f);

    glm::vec3 map_position = glm::vec3(-1.f, -5.f, -30.7703f);
    glm::vec3 map_rotation = glm::vec3(-90.f, 0.f, 0.f);
    glm::vec3 freddy_position = glm::vec3(-1.78686f, 3.0f, -30.7703f);
    glm::vec3 bunny_position = glm::vec3(-1.78686f, -5.f, -20.7703f);

};

extern SceneUniforms SceneData;

struct LightUniforms {
    glm::vec4 La = glm::vec4(0.2f, 0.2f, 0.25f, 1.0f); // ambient light color
    glm::vec4 Ld = glm::vec4(0.5f, 0.5f, 0.25f, 1.0f); // diffuse light color
    glm::vec4 Ls = glm::vec4(0.3f); // specular light color
    glm::vec4 light_w = glm::vec4(0.0f, 2.2, 1.0f, 1.0f); // world-space light position

};

extern LightUniforms LightData;

struct MaterialUniforms {
    glm::vec4 ka = glm::vec4(0.312f, 0.261f, 0.139, 1.0f); // ambient material color
    glm::vec4 kd = glm::vec4(0.861f, 0.663f, 0.518f, 1.0f); // diffuse material color
    glm::vec4 ks = glm::vec4(1.0f); // specular material color
    float shininess = 20.0f; // specular exponent
};
extern MaterialUniforms MaterialData;


extern Pawn gFreddy;
extern Pawn gBunny;
extern std::shared_ptr<SkinnedMesh> gMapMesh;

extern int model_opt;

struct ControllerState {
    glm::vec2 trackpad_left = glm::vec2(0.f, 0.f);
    glm::vec2 trackpad_right;

    bool trackpadClick_left;
    bool trackpadClick_right;

    bool triggerClick_left;
    bool triggerClick_right;

    bool squeezeClick_left;
    bool squeezeClick_right;

    bool menuClick_left;
    bool menuClick_right;

    glm::mat4 pose_left = glm::mat4(1.0f);
    glm::mat4 pose_right = glm::mat4(1.0f);

};

extern ControllerState gControllerState;

extern bool CaptureGui;
extern bool RecordingBuffer;
extern bool ClearDefaultFb;

static const std::string ShaderDir = "shaders/";
//static const std::string MeshDir = "assets/";
//static const std::string TextureDir = "assets/";

static const std::string vertex_shader("template.vert");
static const std::string fragment_shader("template.frag");
//GLuint shader_program = -1;
extern std::shared_ptr<Shader> shader_program;

static const std::string mesh_name = "assets/Amago0.obj";
static const std::string texture_name = "assets/AmagoT.bmp";

static const std::string skinned_vertex_shader("skinning.vert");
static const std::string skinned_fragment_shader("skinning.frag");
//GLuint skinned_shader_program = -1;
extern std::shared_ptr<Shader> skinned_shader_program;
static const std::string map_name = "assets/Map2/Scene.gltf";

static const std::string freddy_model = "assets/Characters/freddy/freddy_ill_walk.gltf";
static const std::string bunny_model = "assets/Characters/bunny/bunny_crawl.gltf";

extern GLuint texture_id; // Texture map for mesh
//MeshData mesh_data;
extern std::shared_ptr<MeshBase> mesh;
//std::shared_ptr<SkinnedMesh> gMapMesh;
//std::shared_ptr<AnimeMesh> freddy;
//std::shared_ptr<AnimeMesh> bunny;

extern float angle;
extern float scale;

extern glm::mat4 Mhmd;
extern float Trigger[2];

// IDs for the buffer objects holding the uniform block data
extern GLuint scene_ubo;
extern GLuint light_ubo;
extern GLuint material_ubo;

namespace UboBinding {
// These values come from the binding value specified in the shader block layout
static const int scene = 0;
static const int light = 1;
static const int material = 2;
}

// Locations for the uniforms which are not in uniform blocks
namespace UniformLocs {
static const int M = 0; // model matrix
static const int time = 1;
}

extern glm::mat4 Mhmd;
extern glm::mat4 Mcontroller[2];
extern float Trigger[2];

void Display(GLFWwindow* window);
void DisplayVr(const glm::mat4& P, const glm::mat4& V);
void Idle();
void Init();
void ModelInit();

}