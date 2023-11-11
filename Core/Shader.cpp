#include "Shader.h"
#include "InitShader.h"

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

int Shader::GetUniformLocation(const char* name)
{
   return glGetUniformLocation(mShader, name);
}

void Shader::UseProgram()
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