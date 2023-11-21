#pragma once

#include <string>
#include <filesystem>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "UniformGui.h"
#include <iostream>
#include <list>

class Shader
{
   public:
      Shader(const std::string& vs_name, const std::string& fs_name);
      Shader(const std::string& vs_name, const std::string& gs_name, const std::string& fs_name);
      ~Shader();
      bool Init();
      bool Reload();
      void UseProgram();
      int GetUniformLocation(const char* name);
      GLuint GetShaderID() { return mShader; }
      void SetMode(int mode);
      void DrawUniformGui(bool& open);
      std::string GetFilename(int i) {assert(i<5); return mFilenames[i];}

      template <typename T>
      void setUniform(const std::string& name, T&& v) const;

      template <typename T>
      void setUniform(const std::string& name, T& v) const;

   protected:
      UniformGuiContext mGuiContext;
      std::string mFilenames[5];
      std::filesystem::file_time_type mTimestamp[5];
      GLuint mShader = -1;
      GLint mModeLoc = -1;
      int mMode = 0;

      bool mEnableDebugBreak = true;

      static const int VS_INDEX = 0;
      static const int TC_INDEX = 1;
      static const int TE_INDEX = 2;
      static const int GS_INDEX = 3;
      static const int FS_INDEX = 4;
      static bool sErrorFlag;

   public:
      static std::list<Shader*>& sAllShaders();
      static void sReloadAll();
      static void ClearError() {sErrorFlag = false;}
      static bool GetError() {return sErrorFlag;}
};
