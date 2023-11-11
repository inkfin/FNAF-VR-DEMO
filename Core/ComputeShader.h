#pragma once

#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace ComputeShaderGui
{
   void Menu();
   void DrawGui();
}

class ComputeShader //TODO inherit from Shader
{
   public:
      ComputeShader(const std::string& filename="");
      virtual ~ComputeShader();

      bool Init();
      bool Reload();
      void UseProgram();
      void Dispatch();

      GLuint GetShader()      {return mShader;}
      std::string GetName()   {return mFilename;}

      glm::ivec3 GetWorkGroupSize() { return mWorkGroupSize; }

      void SetGridSize(glm::ivec3 grid_size);
      glm::ivec3 GetGridSize() {return mGridSize;}
      
      int GetMode()  {return mMode;}
      void SetMode(int mode);

      int GetNumElements() { return mNumElements; }
      void SetNumElements(int n);

      void SetTime(float t);

      int GetUniformLocation(const char* name);
   
      void ExportBinary(const std::string filename = "");

      void Define(const std::string& symbol, const std::string& value = "") {mDefines[symbol] = value;}
      void Undefine(const std::string& symbol) {mDefines.erase(symbol);}

      struct define_value
      {
         std::string mValue = "";
         bool mEnabled = true;

         define_value(){}
         define_value(std::string v):mValue(v){}
      };

      define_value GetDefine(const std::string& symbol)
      {
         auto it = mDefines.find(symbol);
         if (it == mDefines.end())
         {
            return define_value();
         }
         return it->second;
      }
      std::string GenerateDefineCode();

   private:
      void SetWorkGroupSize(glm::ivec3 size);
      std::string mFilename;
      GLuint mShader = -1;
      glm::ivec3 mWorkGroupSize = glm::ivec3(0, 0, 0);
      glm::ivec3 mNumWorkgroups = glm::ivec3(0);
      glm::ivec3 mGridSize = glm::ivec3(0);
      
      GLint mModeLoc = -1;
      int mMode = 0;

      GLint mNumElementsLoc = -1;
      int mNumElements = -1;

      GLint mTimeLoc = -1;
      bool mEnableDebugBreak = true;

      
      std::unordered_map<std::string, define_value> mDefines;

      std::filesystem::file_time_type mTimestamp;

      static bool sErrorFlag;
      static std::list<ComputeShader*>& sAllShaders();
      friend void ComputeShaderGui::Menu();
      friend void ComputeShaderGui::DrawGui();

   public:
      static void sReloadAll();
      static void ClearError() { sErrorFlag = false; }
      static bool GetError() { return sErrorFlag; }
};


