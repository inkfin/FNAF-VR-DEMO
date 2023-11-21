#include "GlfwWindow.h"
#include "GlfwCallbacks.h"
#include <glm/glm.hpp>
#include <sstream>
#include <fstream>
#include <iostream>

GLFWwindow* GlfwWindow::window = nullptr;
const int GlfwWindow::InitSize[2] = { 1024, 768 };
float GlfwWindow::Aspect = float(InitSize[0]) / float(InitSize[1]);
int GlfwWindow::Size[2] = { InitSize[0], InitSize[1]};

void GlfwWindow::Create()
{
    /* Initialize the library */
    if (!glfwInit())
    {
        //return -1;
        return;
    }
    //glfwSetErrorCallback(glfw_error);

    //request stencil buffer
    /*
    if (enable_stencil)
    {
        glfwWindowHint(GLFW_STENCIL_BITS, 8);
    }

    //Request multisample framebuffer before window is created
    if (enable_msaa)
    {
        glfwWindowHint(GLFW_SAMPLES, msaa_samples);
    }
    */

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    GLFWmonitor* prim_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(prim_monitor);
    //screen_size.x = mode->width;
    //screen_size.y = mode->height;

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    GLFWmonitor* monitor = nullptr;
    /*
    if (fullscreen == true)
    {
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        monitor = prim_monitor;
    }
    */
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(InitSize[0], InitSize[1], PROJECT_NAME, monitor, nullptr);

    if (!window)
    {
        glfwTerminate();
        //return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Register callbacks before ImGui Init
    GlfwCallbacks::Register(GlfwWindow::window);

}

void GlfwWindow::OpenGlCaps()
{
    //Print out information about the OpenGL version supported by the graphics driver.	
    std::ostringstream oss;
    oss << "GL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
    oss << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
    oss << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
    oss << "GL_SHADING_LANGUAGE_VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    int max_invocations = 0;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_invocations);
    oss << "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS: " << max_invocations << std::endl;

    glm::ivec3 max_work_group_count;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_work_group_count.x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_work_group_count.y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_work_group_count.z);
    oss << "GL_MAX_COMPUTE_WORK_GROUP_COUNT: "
        << max_work_group_count.x << ", "
        << max_work_group_count.y << ", "
        << max_work_group_count.z << std::endl;

    glm::ivec3 max_work_group_size;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &max_work_group_size.x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &max_work_group_size.y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &max_work_group_size.z);
    oss << "GL_MAX_COMPUTE_WORK_GROUP_SIZE: "
        << max_work_group_size.x << ", "
        << max_work_group_size.y << ", "
        << max_work_group_size.z << std::endl;

    int shared_mem_size = 0;
    glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &shared_mem_size);
    oss << "GL_MAX_COMPUTE_SHARED_MEMORY_SIZE: " << shared_mem_size << std::endl;

    int max_uniform_blocks = 0;
    glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &max_uniform_blocks);
    oss << "GL_MAX_COMPUTE_UNIFORM_BLOCKS: " << max_uniform_blocks << std::endl;

    int max_uniform_block_size = 0;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_block_size);
    oss << "GL_MAX_UNIFORM_BLOCK_SIZE: " << max_uniform_block_size << std::endl;

    int max_storage_blocks = 0;
    glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &max_storage_blocks);
    oss << "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS: " << max_storage_blocks << std::endl;

    int max_storage_block_size = 0;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_storage_block_size);
    oss << "GL_MAX_SHADER_STORAGE_BLOCK_SIZE: " << max_storage_block_size << std::endl;

    int max_compute_texture_image_units = 0;
    glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &max_compute_texture_image_units);
    oss << "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS: " << max_compute_texture_image_units << std::endl;

    int max_texture_size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    oss << "GL_MAX_TEXTURE_SIZE: " << max_texture_size << std::endl;

    int max_3d_texture_size = 0;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_3d_texture_size);
    oss << "GL_MAX_3D_TEXTURE_SIZE: " << max_3d_texture_size << std::endl;

#ifdef GL_SUBGROUP_SIZE_KHR
    int subgroup_size = 0; //warp/wavefront size
    glGetIntegerv(GL_SUBGROUP_SIZE_KHR, &subgroup_size);
    oss << "GL_SUBGROUP_SIZE_KHR: " << subgroup_size << std::endl;
#endif

    //Output to console and file
    std::cout << oss.str();

    std::fstream info_file("info.txt", std::ios::out | std::ios::trunc);
    info_file << oss.str();
    info_file.close();

}