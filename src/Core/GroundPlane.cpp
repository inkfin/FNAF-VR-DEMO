#include "GroundPlane.h"
#include "InitShader.h"
#include "AttriblessRendering.h"
#include <string>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


static const std::string vertex_shader("ground_vs.glsl");
static const std::string fragment_shader("ground_fs.glsl");


void GroundPlane::Init()
{
    mShader = InitShader(vertex_shader.c_str(), fragment_shader.c_str());
    //float largest_supported_anisotropy;
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);

    //glBindTexture(GL_TEXTURE_2D, texId);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);
}

void GroundPlane::Display()
{
    glUseProgram(mShader);
    bind_attribless_vao();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


GroundPlaneModule::GroundPlaneModule():mShader(vertex_shader.c_str(), fragment_shader.c_str())
{

}

void GroundPlaneModule::Init()
{
    mShader.Init();
}

void GroundPlaneModule::Draw()
{
    mShader.UseProgram();
    bind_attribless_vao();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


