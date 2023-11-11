#include <GL/glew.h>
#include "InitShader.h"
#include "ShaderInclude.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
//Adapted from Edward Angel's InitShader code

namespace
{
   static string ShaderDir = "";

   static string CodeInjection = "";

   void injectCode(std::string& code)
   {
      if(CodeInjection.length()==0) return;

      const std::string version = "#version";
      const std::string extension = "#extension";
      const std::string newline = "\n";

      //find position of last #version or #extension
      std::size_t inject_pos = code.rfind(version);
      std::size_t ext_pos = code.rfind(extension);
      if(ext_pos != string::npos) inject_pos = ext_pos;

      if(inject_pos != string::npos)
      {
         //find end of the line containing #version of #extension
         std::size_t next_end = code.find(newline, inject_pos);
         if(next_end != string::npos) inject_pos = next_end+1;
      }
      else
      {
         inject_pos = 0;
      }
      //inject the code
      code.insert(inject_pos, CodeInjection);
   }

   struct Shader
   {
      std::string filename;
      GLenum   type;
      std::string source;
      GLuint shader_id;
   };

   // Create a NULL-terminated string by reading the provided file
   static char* readShaderSource(const char* shaderFile)
   {
      ifstream ifs(shaderFile, ios::in | ios::binary | ios::ate);
      if (ifs.is_open())
      {
         unsigned int filesize = static_cast<unsigned int>(ifs.tellg());
         ifs.seekg(0, ios::beg);
         char* bytes = new char[filesize + 1];
         memset(bytes, 0, filesize + 1);
         ifs.read(bytes, filesize);
         ifs.close();
         return bytes;
      }
      return NULL;
   }

   void printShaderCompileError(GLuint shader)
   {
      GLint  logSize;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
      char* logMsg = new char[logSize];
      glGetShaderInfoLog(shader, logSize, NULL, logMsg);
      std::cerr << logMsg << std::endl;
      delete[] logMsg;
   }

   void printProgramLinkError(GLuint program)
   {
      GLint  logSize;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
      char* logMsg = new char[logSize];
      glGetProgramInfoLog(program, logSize, NULL, logMsg);
      std::cerr << logMsg << std::endl;
      delete[] logMsg;
   }

   int loadShaderFile(Shader& s)
   {
      s.filename = ShaderDir+s.filename;
      s.source = ShaderInclude::load(s.filename);

      if (s.source.length() == 0)
      {
         std::cerr << "Failed to read " << s.filename << std::endl;
         return -1;
      }

      //insert the code injection after #version and #extension
      injectCode(s.source);

      

      s.shader_id = glCreateShader(s.type);
      const char* c_str = s.source.c_str();
      glShaderSource(s.shader_id, 1, (const GLchar**)&c_str, NULL);
      glCompileShader(s.shader_id);

      GLint compiled;
      glGetShaderiv(s.shader_id, GL_COMPILE_STATUS, &compiled);
      if (!compiled)
      {
         std::cerr << s.filename << " failed to compile:" << std::endl;
         printShaderCompileError(s.shader_id);
         glDeleteShader(s.shader_id);
         return -1;
      }

      return s.shader_id;
   }

   bool linkProgram(int program)
   {
      /* link and error check */
      glLinkProgram(program);

      GLint linked;
      glGetProgramiv(program, GL_LINK_STATUS, &linked);
      if (!linked)
      {
         std::cerr << "Shader program failed to link" << std::endl;
         printProgramLinkError(program);
         return false;
      }
      return true;
   }
};

void SetShaderDir(const std::string& dir)
{
   ShaderDir = dir;
}

const std::string GetShaderDir() { return ShaderDir; }

void SetCodeInjection(const std::string& inj = "")
{
   CodeInjection = inj;
}

const std::string GetCodeInjection()
{
   return CodeInjection;
}

void ClearCodeInjection()
{
   CodeInjection = "";
}

GLuint InitShader(const std::string& computeShaderFile)
{
   Shader shaders = { computeShaderFile, GL_COMPUTE_SHADER, "", -1 };
   
   GLuint program = glCreateProgram();

   GLuint shader_id = loadShaderFile(shaders);
   if (shader_id != -1)
   {
      glAttachShader(program, shader_id);
   }
   
   bool linked = linkProgram(program);
   if (linked == false)
   {
      glDeleteProgram(program);
      return -1;
   }

   if(shader_id != 0)
   {
      glDeleteShader(shader_id);
   }
   
   /* use program object */
   glUseProgram(program);
   return program;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
   return InitShader(vertexShaderFile, "", "", "", fragmentShaderFile);
}

// Create a GLSL program object from vertex, geometry and fragment shader files
GLuint InitShader(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile)
{
   return InitShader(vertexShaderFile, "", "", geometryShaderFile, fragmentShaderFile);
}

GLuint InitShader(const std::string& vertexShaderFile, const std::string& tessControlShaderFile, const std::string& tessEvalShaderFile, const std::string& fragmentShaderFile)
{
   return InitShader(vertexShaderFile, tessControlShaderFile, tessEvalShaderFile, "", fragmentShaderFile);
}

GLuint InitShader(const std::string& vertexShaderFile, const std::string& tessControlShaderFile, const std::string& tessEvalShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile)
{
   const int NUM_FILES = 5;
   Shader shaders[NUM_FILES] =
   {
      { vertexShaderFile, GL_VERTEX_SHADER, "", -1},
      { tessControlShaderFile, GL_TESS_CONTROL_SHADER, "", -1},
      { tessEvalShaderFile, GL_TESS_EVALUATION_SHADER, "", -1},
      { geometryShaderFile, GL_GEOMETRY_SHADER, "", -1},
      { fragmentShaderFile, GL_FRAGMENT_SHADER, "", -1}
   };

   GLuint program = glCreateProgram();
   bool shader_success = true;
   for (int i = 0; i < NUM_FILES; ++i)
   {
      if(shaders[i].filename != "")
      {
         GLuint shader_id = loadShaderFile(shaders[i]);
         if (shader_id == -1)
         {
            shader_success = false;
         }
         else
         {
            glAttachShader(program, shader_id);
         }
      }
   }

   bool linked = linkProgram(program);
   for (int i = 0; i < NUM_FILES; i++)
   {
      if (shaders[i].shader_id != -1)
      {
         glDeleteShader(shaders[i].shader_id);
      }
   }

   if (linked == false || shader_success == false)
   {
      glDeleteProgram(program);
      return -1;
   }

   /* use program object */
   glUseProgram(program);
   return program;
}

