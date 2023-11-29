#include "Shader.h"
#include "InitShader.h"

#include <GL/glew.h>

#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

bool Shader::sErrorFlag = false;

Shader::Shader(const std::string& vs_name, const std::string& fs_name)
{
   mFilenames[VS_INDEX] = vs_name;
   mFilenames[FS_INDEX] = fs_name;
   sAllShaders().push_back(this);
}

Shader::Shader(const std::string& vs_name, const std::string& gs_name, const std::string& fs_name)
{
   mFilenames[VS_INDEX] = vs_name;
   mFilenames[GS_INDEX] = gs_name;
   mFilenames[FS_INDEX] = fs_name;
   sAllShaders().push_back(this);
}

Shader::~Shader()
{
   if (!sAllShaders().empty()) {
       sAllShaders().remove(this);
   }
}

void Shader::ClearAllShaders()
{
    sAllShaders().clear();
}


bool Shader::Init()
{
   bool success = true;
   
   GLuint new_shader = InitShader(mFilenames[0], mFilenames[1], mFilenames[2], mFilenames[3], mFilenames[4]);

   if (new_shader == -1) // loading failed
   {
      success = false;
      sErrorFlag = true;

      if (mEnableDebugBreak == true)
      {
         DebugBreak();  //Check text console for error messages 
         mEnableDebugBreak = false; //This allows execution to continue after the break. Breaks won't happen for subsequent errors
      }
   }
   else //success
   {
      mEnableDebugBreak = true;
      if (mShader != -1)
      {
         glDeleteProgram(mShader);
      }
      mShader = new_shader;
      mModeLoc = glGetUniformLocation(mShader, "uMode");
      SetMode(mMode);
      mGuiContext.Init(mShader);
      if(mFilenames[0] != "")
      {
         mGuiContext.mTitle = mFilenames[0];
      }

      //Set timestamps
      std::string shader_dir = GetShaderDir();
      for (int i = 0; i < 5; i++)
      {
         if (mFilenames[i] != "")
         {
            std::filesystem::path filepath(shader_dir + mFilenames[i]);
            std::error_code ec;
            mTimestamp[i] = std::filesystem::last_write_time(filepath, ec);
            //TODO: handle error code
            std::cout << "Shader filepath: " << filepath << std::endl;
         }
      }
   }
   return success;
}

bool Shader::Reload()
{
   //get current timestamps
   bool needs_init = false;
   std::filesystem::file_time_type ftime[5];
   std::string shader_dir = GetShaderDir();
   for (int i = 0; i < 5; i++)
   {
      if (mFilenames[i] != "")
      {
         std::filesystem::path filepath(shader_dir + mFilenames[i]);
         std::error_code ec;
         ftime[i] = std::filesystem::last_write_time(filepath, ec);
         //TODO: handle error code
         if (ftime[i] > mTimestamp[i])
         {
            needs_init = true;
         }
      }
   }

   if (needs_init == false) return false;
   bool success = Init();

   return success;
}

std::list<Shader*>& Shader::sAllShaders()
{
   static std::list<Shader*> all_shaders;
   return all_shaders;
}

void Shader::sReloadAll()
{
   for (Shader* pShader : sAllShaders())
   {
      pShader->Reload();
   }
}

int Shader::GetUniformLocation(const char* name) const
{
   return glGetUniformLocation(mShader, name);
}

void Shader::UseProgram() const
{
   glUseProgram(mShader);
}

void Shader::SetMode(int mode)
{
   mMode = mode;
   if (mModeLoc >= 0 && mShader >= 0)
   {
      glProgramUniform1i(mShader, mModeLoc, mMode);
   }
}

void Shader::DrawUniformGui(bool& open)
{
   if(open == true)
   {
      UniformGui(mGuiContext, open);
   }
}

// define helper macro for setting shader uniforms
#define GL_SET_UNIFORM_V(num, type, value) \
    glUniform##num##type##v(glGetUniformLocation(mShader, name.c_str()), 1, value)
#define GL_SET_UNIFORM_M(num, type, value) \
    glUniformMatrix##num##type##v(glGetUniformLocation(mShader, name.c_str()), 1, GL_FALSE, value)

/////////////////////////////////////////////////////
template <typename T>
void Shader::setUniform(const std::string& name, T&& v) const
{
    throw std::runtime_error("Class Shader::setUniform load unsupported class!");
}

template <>
void Shader::setUniform<bool>(const std::string& name, bool&& v) const
{
    glUniform1i(glGetUniformLocation(mShader, name.c_str()), static_cast<int>(v));
}

template <>
void Shader::setUniform<int>(const std::string& name, int&& v) const
{
    glUniform1i(glGetUniformLocation(mShader, name.c_str()), v);
}

template <>
void Shader::setUniform<float>(const std::string& name, float&& v) const
{
    glUniform1f(glGetUniformLocation(mShader, name.c_str()), v);
}

template <>
void Shader::setUniform<glm::vec2>(const std::string& name, glm::vec2&& v) const
{
    __glewUniform2fv(__glewGetUniformLocation(mShader, name.c_str()), 1, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::vec3>(const std::string& name, glm::vec3&& v) const
{
    GL_SET_UNIFORM_V(3, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::vec4>(const std::string& name, glm::vec4&& v) const
{
    GL_SET_UNIFORM_V(4, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat2>(const std::string& name, glm::mat2&& v) const
{
    GL_SET_UNIFORM_M(2, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat3>(const std::string& name, glm::mat3&& v) const
{
    GL_SET_UNIFORM_M(3, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat4>(const std::string& name, glm::mat4&& v) const
{
    GL_SET_UNIFORM_M(4, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat2x3>(const std::string& name, glm::mat2x3&& v) const
{
    GL_SET_UNIFORM_M(2x3, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat3x2>(const std::string& name, glm::mat3x2&& v) const
{
    GL_SET_UNIFORM_M(3x2, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat2x4>(const std::string& name, glm::mat2x4&& v) const
{
    GL_SET_UNIFORM_M(2x4, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat4x2>(const std::string& name, glm::mat4x2&& v) const
{
    GL_SET_UNIFORM_M(4x2, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat3x4>(const std::string& name, glm::mat3x4&& v) const
{
    GL_SET_UNIFORM_M(3x4, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat4x3>(const std::string& name, glm::mat4x3&& v) const
{
    GL_SET_UNIFORM_M(4x3, f, glm::value_ptr(v));
}

/////////////////////////////////////////////////////

template <typename T>
void Shader::setUniform(const std::string& name, T& v) const
{
    throw std::runtime_error("Class Shader::setUniform load unsupported class!");
}

template <>
void Shader::setUniform<bool>(const std::string& name, bool& v) const
{
    glUniform1i(glGetUniformLocation(mShader, name.c_str()), static_cast<int>(v));
}

template <>
void Shader::setUniform<int>(const std::string& name, int& v) const
{
    glUniform1i(glGetUniformLocation(mShader, name.c_str()), v);
}

template <>
void Shader::setUniform<float>(const std::string& name, float& v) const
{
    glUniform1f(glGetUniformLocation(mShader, name.c_str()), v);
}

template <>
void Shader::setUniform<glm::vec2>(const std::string& name, glm::vec2& v) const
{
    // glUniform3fv(glGetUniformLocation(mShader, name.c_str()), 1, glm::value_ptr(v));
    GL_SET_UNIFORM_V(2, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::vec3>(const std::string& name, glm::vec3& v) const
{
    // glUniform3fv(glGetUniformLocation(mShader, name.c_str()), 1, glm::value_ptr(v));
    GL_SET_UNIFORM_V(3, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::vec4>(const std::string& name, glm::vec4& v) const
{
    // glUniform3fv(glGetUniformLocation(mShader, name.c_str()), 1, glm::value_ptr(v));
    GL_SET_UNIFORM_V(4, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat2>(const std::string& name, glm::mat2& v) const
{
    GL_SET_UNIFORM_M(2, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat3>(const std::string& name, glm::mat3& v) const
{
    GL_SET_UNIFORM_M(3, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat4>(const std::string& name, glm::mat4& v) const
{
    GL_SET_UNIFORM_M(4, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat2x3>(const std::string& name, glm::mat2x3& v) const
{
    GL_SET_UNIFORM_M(2x3, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat3x2>(const std::string& name, glm::mat3x2& v) const
{
    GL_SET_UNIFORM_M(3x2, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat2x4>(const std::string& name, glm::mat2x4& v) const
{
    GL_SET_UNIFORM_M(2x4, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat4x2>(const std::string& name, glm::mat4x2& v) const
{
    GL_SET_UNIFORM_M(4x2, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat3x4>(const std::string& name, glm::mat3x4& v) const
{
    GL_SET_UNIFORM_M(3x4, f, glm::value_ptr(v));
}

template <>
void Shader::setUniform<glm::mat4x3>(const std::string& name, glm::mat4x3& v) const
{
    GL_SET_UNIFORM_M(4x3, f, glm::value_ptr(v));
}

/////////////////////////////////////////////////////
#undef GL_SET_UNIFORM_V
#undef GL_SET_UNIFORM_M