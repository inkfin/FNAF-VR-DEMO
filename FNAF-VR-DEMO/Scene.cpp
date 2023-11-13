#include "Scene.h"
#include "Game.h"
#include "GlfwWindow.h"
#include <string>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "DrawGui.h"

#include "InitShader.h" //Functions for loading shaders from text files
//#include "LoadMesh.h" //Functions for creating OpenGL buffers from mesh files
#include "LoadTexture.h" //Functions for creating OpenGL textures from image files
#include "MeshBase.h"
#include "SkinnedMesh.h"

namespace Scene {
bool CaptureGui = true;
bool RecordingBuffer = false;
bool ClearDefaultFb = true;
std::string ShaderDir = "shaders/";
//std::string MeshDir = "assets/";
//std::string TextureDir = "assets/";

static const std::string vertex_shader("template.vert");
static const std::string fragment_shader("template.frag");
GLuint shader_program = -1;

static const std::string mesh_name = "assets/Amago0.obj";
static const std::string texture_name = "assets/AmagoT.bmp";


static const std::string skinned_vertex_shader("skinning.vert");
static const std::string skinned_fragment_shader("skinning.frag");
GLuint skinned_shader_program = -1;
static const std::string map_name = "assets/Map/Scene.dae";

GLuint texture_id = -1; // Texture map for mesh
//MeshData mesh_data;
std::shared_ptr<MeshBase> mesh;
std::shared_ptr<SkinnedMesh> skinned_mesh;

float angle = 0.0f;
float scale = 1.0f;

glm::mat4 Mhmd(1.0f);
glm::mat4 Mcontroller[2] = { glm::mat4(1.0f), glm::mat4(1.0) };
float Trigger[2] = { 0.0f, 0.0f };

// This structure mirrors the uniform block declared in the shader
struct SceneUniforms {
    glm::mat4 P = glm::perspective(glm::pi<float>() / 4.0f, GlfwWindow::Aspect, 0.1f, 100.0f);
    glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 PV; // camera projection * view matrix
    glm::vec4 eye_w = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f); // world-space eye position
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.f, 3.0f));
    float rotation_angle = 135.f;
    glm::vec3 initial_location = glm::vec3(4.29809f, -5.f, -29.f);
} SceneData;

struct LightUniforms {
    glm::vec4 La = glm::vec4(0.5f, 0.5f, 0.55f, 1.0f); // ambient light color
    glm::vec4 Ld = glm::vec4(0.5f, 0.5f, 0.25f, 1.0f); // diffuse light color
    glm::vec4 Ls = glm::vec4(0.3f); // specular light color
    glm::vec4 light_w = glm::vec4(0.0f, 1.2, 1.0f, 1.0f); // world-space light position

} LightData;

struct MaterialUniforms {
    glm::vec4 ka = glm::vec4(1.0f); // ambient material color
    glm::vec4 kd = glm::vec4(1.0f); // diffuse material color
    glm::vec4 ks = glm::vec4(1.0f); // specular material color
    float shininess = 20.0f; // specular exponent
} MaterialData;

// IDs for the buffer objects holding the uniform block data
GLuint scene_ubo = -1;
GLuint light_ubo = -1;
GLuint material_ubo = -1;

namespace UboBinding {
    // These values come from the binding value specified in the shader block layout
    int scene = 0;
    int light = 1;
    int material = 2;
}

// Locations for the uniforms which are not in uniform blocks
namespace UniformLocs {
    int M = 0; // model matrix
    int time = 1;
}
}

namespace Camera {

void UpdateP()
{
}

void UpdateV(glm::vec3 delta_pos)
{
    glm::mat4 rotation = glm::inverse(Scene::SceneData.V);
    rotation[3][0] = rotation[3][1] = rotation[3][2] = 0.f;
    delta_pos = (glm::vec3)(rotation * glm::vec4(delta_pos, 0.f));
    Scene::SceneData.translation *= glm::translate(delta_pos);
    std::cout << "Scene::SceneData.translation: " << Scene::SceneData.translation[3][0] << ", " << Scene::SceneData.translation[3][1] << ", " << Scene::SceneData.translation[3][2] << "" << std::endl;
}
};

void Scene::Display(GLFWwindow* window)
{
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);
    if (ClearDefaultFb == true) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    //SceneData.eye_w = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f);
    glm::mat4 M = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(scale * mesh->mScaleFactor);
    //SceneData.V = glm::lookAt(glm::vec3(SceneData.eye_w), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //SceneData.P = glm::perspective(glm::pi<float>() / 4.0f, GlfwWindow::Aspect, 0.1f, 100.0f);
    //SceneData.PV = SceneData.P * SceneData.V;

    glUseProgram(shader_program);

    // Note that we don't need to set the value of a uniform here. The value is set with the "binding" in the layout qualifier
    glBindTextureUnit(0, texture_id);

    glBindBuffer(GL_UNIFORM_BUFFER, scene_ubo); // Bind the OpenGL UBO before we update the data.
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneData), &SceneData); // Upload the new uniform values.
    glBindBuffer(GL_UNIFORM_BUFFER, 0); // unbind the ubo

    glBindVertexArray(mesh->mVao);
    glUniformMatrix4fv(UniformLocs::M, 1, false, glm::value_ptr(M));
    glDrawElements(GL_TRIANGLES, mesh->mSubmesh[0].mNumIndices, GL_UNSIGNED_INT, 0);

    glUseProgram(skinned_shader_program);

    glm::mat4 RS = glm::rotate(SceneData.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::vec3(1.f, -1.f, 1.f));
    glm::mat4 M2 = glm::translate(SceneData.initial_location) * RS;
    
    glUniformMatrix4fv(glGetUniformLocation(skinned_shader_program, "PV"), 1, false, glm::value_ptr(SceneData.PV));
    glUniformMatrix4fv(glGetUniformLocation(skinned_shader_program, "M"), 1, false, glm::value_ptr(M2));
    skinned_mesh->Render();

    // Swap front and back buffers
    glfwSwapBuffers(window);
}

void Scene::DisplayVr(const glm::mat4& P, const glm::mat4& V)
{
    // No clear in this function

    glm::mat4 M = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(scale * mesh->mScaleFactor);
    SceneData.P = P;
    
    glm::mat4 rotation = glm::inverse(V);
    rotation[3][0] = rotation[3][1] = rotation[3][2] = 0.f;
    SceneData.V = V * Scene::SceneData.translation;
    SceneData.PV = SceneData.P * SceneData.V;

    glUseProgram(shader_program);

    // Note that we don't need to set the value of a uniform here. The value is set with the "binding" in the layout qualifier
    glBindTextureUnit(0, texture_id);

    // Set uniforms
    glUniformMatrix4fv(UniformLocs::M, 1, false, glm::value_ptr(M));

    glBindBuffer(GL_UNIFORM_BUFFER, scene_ubo); // Bind the OpenGL UBO before we update the data.
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneData), &SceneData); // Upload the new uniform values.
    glBindBuffer(GL_UNIFORM_BUFFER, 0); // Unbind the ubo

    glBindVertexArray(mesh->mVao);

    glm::mat4 S[2] = { glm::scale(glm::vec3(Trigger[0] + 0.1f)), glm::scale(glm::vec3(Trigger[1] + 0.1f)) };

    for (glm::mat4 Mi : { M, Mcontroller[0] * S[0], Mcontroller[1] * S[1], Mhmd }) {
        glUniformMatrix4fv(UniformLocs::M, 1, false, glm::value_ptr(Mi));
        glDrawElements(GL_TRIANGLES, mesh->mSubmesh[0].mNumIndices, GL_UNSIGNED_INT, 0);
    }

    glUseProgram(skinned_shader_program);

    glm::mat4 RS = glm::rotate(SceneData.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::vec3(1.f, -1.f, 1.f));
    glm::mat4 M2 = glm::translate(SceneData.initial_location) * RS;
    glUniformMatrix4fv(glGetUniformLocation(skinned_shader_program, "PV"), 1, false, glm::value_ptr(SceneData.PV));
    glUniformMatrix4fv(glGetUniformLocation(skinned_shader_program, "M"), 1, false, glm::value_ptr(M2));
    skinned_mesh->Render();
    
    // No swap buffers in this function
}

void Scene::Idle()
{
    constexpr float fixed_time_step = 1.0f / 60.0f;
    static float time_passed = 0.0f; // update fixed step every 1/60 sec

    static float prev_time_sec = 0.0f;
    const float time_sec = static_cast<float>(glfwGetTime());
    const float dt = time_sec - prev_time_sec;

    Game::UpdateDynamicStep(dt);

    int max_iter = 10;
    while (time_passed >= fixed_time_step && --max_iter) {
        Game::UpdateFixedStep();
        time_passed -= fixed_time_step;
    }

    prev_time_sec = time_sec;
    time_passed += dt;

    skinned_mesh->Update(time_sec);
    glUniform1f(glGetUniformLocation(skinned_shader_program, "time"), time_sec);
}

void Scene::Init()
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

    ClearDefaultFb = false;

    //mesh_data = LoadMesh(mesh_name);
    mesh = std::make_shared<MeshBase>(mesh_name);
    texture_id = LoadTexture(texture_name);
    shader_program = InitShader(vertex_shader.c_str(), fragment_shader.c_str());

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

    skinned_shader_program = InitShader(skinned_vertex_shader.c_str(), skinned_fragment_shader.c_str());
    skinned_mesh = std::make_shared<SkinnedMesh>();
    skinned_mesh->LoadMesh(map_name);

    // glGetIntegerv(GL_VIEWPORT, &SceneData.Viewport[0]);
    // Camera::UpdateV();
    // Camera::UpdateP();
    // DrawGui::InitVr();
}