#include "ComputeShader.h"
#include "InitShader.h"
#include <fstream>

bool ComputeShader::sErrorFlag = false;

ComputeShader::ComputeShader(const std::string& filename) :mFilename(filename)
{
   sAllShaders().push_back(this);
}

ComputeShader::~ComputeShader()
{
   sAllShaders().remove(this);
}

std::list<ComputeShader*>& ComputeShader::sAllShaders()
{
   static std::list<ComputeShader*> all_shaders;
   return all_shaders;
}

bool ComputeShader::Init()
{
   bool success = true;

   SetCodeInjection(GenerateDefineCode());
   GLuint new_shader = InitShader(mFilename.c_str());
   ClearCodeInjection();

   if (new_shader == -1) //InitShader fail
   {
      success = false;
      sErrorFlag = true;
      
      if (mEnableDebugBreak == true)
      {
         DebugBreak();  //Check text console for error messages 
         mEnableDebugBreak = false; //This allows execution to continue after the break. Breaks won't happen for subsequent errors
      }
   }
   else //InitShader success
   {
      mEnableDebugBreak = true;
      if (mShader != -1)
      {
         glDeleteProgram(mShader);
      }

      mShader = new_shader;

      mModeLoc = glGetUniformLocation(mShader, "uMode");
      SetMode(mMode);

      //default num elements
      if (mNumElements == -1)
      {
         mNumElements = mGridSize.x * mGridSize.y * mGridSize.z;
      }
      mNumElementsLoc = glGetUniformLocation(mShader, "uNumElements");
      SetNumElements(mNumElements);

      mTimeLoc = glGetUniformLocation(mShader, "uTime");
      SetTime(0.0f);

      glm::ivec3 size;
      glGetProgramiv(mShader, GL_COMPUTE_WORK_GROUP_SIZE, &size.x);
      SetWorkGroupSize(size);

      //Set timestamps
      std::string shader_dir = GetShaderDir();
      std::filesystem::path filepath(shader_dir + mFilename);
      mTimestamp = std::filesystem::last_write_time(filepath);
   }
   return success;
}

bool ComputeShader::Reload()
{
   //get current timestamp
   bool needs_init = false;
   std::filesystem::file_time_type ftime;
   std::string shader_dir = GetShaderDir();
   std::filesystem::path filepath(shader_dir + mFilename);
   ftime = std::filesystem::last_write_time(filepath);
   if (ftime > mTimestamp)
   {
      needs_init = true;
   }

   if (mDefines.empty() == false)
   {
      needs_init = true;
   }
   
   if (needs_init == false) return false;
   bool success = Init();

   return success;
}

void ComputeShader::sReloadAll()
{
   for (ComputeShader* pShader : sAllShaders())
   {
      pShader->Reload();
   }
}

int ComputeShader::GetUniformLocation(const char* name)
{
   return glGetUniformLocation(mShader, name);
}

void ComputeShader::SetGridSize(glm::ivec3 grid_size)
{
   mGridSize = grid_size;
   mNumWorkgroups = glm::ceil(glm::vec3(mGridSize) / glm::vec3(mWorkGroupSize));
   mNumWorkgroups = glm::max(mNumWorkgroups, glm::ivec3(1));
}

void ComputeShader::SetWorkGroupSize(glm::ivec3 size)
{ 
   mWorkGroupSize = size; 
   mNumWorkgroups = glm::ceil(glm::vec3(mGridSize) / glm::vec3(mWorkGroupSize));
   mNumWorkgroups = glm::max(mNumWorkgroups, glm::ivec3(1));
}

void ComputeShader::UseProgram()
{
   glUseProgram(mShader);
}

void ComputeShader::Dispatch()
{
   glDispatchCompute(mNumWorkgroups.x, mNumWorkgroups.y, mNumWorkgroups.z);
}

void ComputeShader::SetMode(int mode)
{
   mMode = mode;
   if (mModeLoc >= 0 && mShader >= 0)
   {
      glProgramUniform1i(mShader, mModeLoc, mMode);
   }
}

void ComputeShader::SetTime(float t)
{
   if (mTimeLoc >= 0 && mShader >= 0)
   {
      glProgramUniform1f(mShader, mTimeLoc, t);
   }
}

void ComputeShader::SetNumElements(int n)
{
   mNumElements = n;
   if (mNumElementsLoc >= 0 && mShader >= 0)
   {
      glProgramUniform1i(mShader, mNumElementsLoc, mNumElements);
   }
}

void ComputeShader::ExportBinary(const std::string filename)
{
   if(mShader==-1) return;

   int bin_formats;
   glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &bin_formats);
   if(bin_formats<1) return;

   glProgramParameteri(mShader, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);

   int bin_len;
   glGetProgramiv(mShader, GL_PROGRAM_BINARY_LENGTH, &bin_len);
   std::string binary;
   binary.resize(bin_len);

   int len;
   GLenum format;
   glGetProgramBinary(mShader, bin_len, &len, &format, binary.data());

   std::string outfilename = filename;
   if (outfilename.length() == 0)
   {
      outfilename = GetShaderDir() + mFilename + std::string(".bin");
   }

   std::fstream bin_file(outfilename, std::ios::out | std::ios::trunc);
   bin_file << binary;
   bin_file.close();
}

std::string ComputeShader::GenerateDefineCode()
{
   std::string code;
   for (auto& [symbol, value] : mDefines)
   {
      if(value.mEnabled==true)
      {
         code += "#define " + symbol + " " + value.mValue + "\n";
      }
   }
   return code;
}

#include <deque>
namespace ComputeShaderGui
{
   std::deque<bool> open; //std::vector<bool> is broken
}

#include "imgui.h"
#include "UniformGui.h"

void ComputeShaderGui::Menu()
{
   if(open.size() != ComputeShader::sAllShaders().size())
   {
      open.resize(ComputeShader::sAllShaders().size(), false);
   }

   if (ImGui::BeginMenu("Compute Shaders"))
   {
      int i=0;
      for(ComputeShader* cs : ComputeShader::sAllShaders())
      {
         bool enabled = cs->mShader != -1;
         if (ImGui::MenuItem(cs->mFilename.c_str(), "", open[i], enabled))
         {
            open[i] = !open[i];
         }
         i++;
      }
      ImGui::EndMenu();
   }
}

void ComputeShaderGui::DrawGui()
{
   if(open.size() == 0) return;
   int i = 0;
   for (ComputeShader* cs : ComputeShader::sAllShaders())
   {
      if (cs->mShader != -1 && open[i] == true)
      {
         ImGui::Begin(cs->mFilename.c_str(), &open[i]);
         ImGui::Text("Shader id = %d", cs->mShader);
         ImGui::Text("Local workgroup size = %d, %d, %d", cs->mWorkGroupSize.x, cs->mWorkGroupSize.y, cs->mWorkGroupSize.z);
         if (ImGui::Button("Reload"))
         {
            cs->Init();
         }
         ImGui::SameLine();
         if (ImGui::Button("Export binary"))
         {
            cs->ExportBinary();
         }

         if (ImGui::CollapsingHeader("#defines"))
         {
            if (ImGui::BeginTable("##DEFINES", 4))
            {
               ImGui::TableSetupColumn("#define symbol", ImGuiTableColumnFlags_None);
               ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_None);
               ImGui::TableSetupColumn("Add/remove", ImGuiTableColumnFlags_None);
               ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_None);
               ImGui::TableHeadersRow();
               int d = 0;
               std::string symbol_to_delete = "";
               for (auto& [symbol, value] : cs->mDefines)
               {
                  ImGui::TableNextRow();
                  ImGui::BeginDisabled(!value.mEnabled);
                  ImGui::TableNextColumn();
                     ImGui::Text(symbol.c_str());
                  ImGui::TableNextColumn();
                     ImGui::Text(value.mValue.c_str());
                  ImGui::EndDisabled();
                  ImGui::TableNextColumn();
                     ImGui::PushID(d);
                     if (ImGui::Button("Remove"))
                     {
                        symbol_to_delete = symbol;
                     }
                     ImGui::PopID();
                  ImGui::TableNextColumn();
                     ImGui::PushID(d);
                     ImGui::Checkbox("Enabled", &value.mEnabled);
                     ImGui::PopID();
                  d++;
               }

               if (symbol_to_delete != "")
               {
                  cs->mDefines.erase(symbol_to_delete);
               }

               const int buf_size = 32;
               static char new_symbol[buf_size], new_value[buf_size];

               ImGui::TableNextRow();
               ImGui::TableNextColumn();
                  ImGui::InputText("##1", new_symbol, buf_size);
               ImGui::TableNextColumn();
                  ImGui::InputText("##2", new_value, buf_size);
               ImGui::TableNextColumn();
               if (ImGui::Button("Add"))
               {
                  cs->mDefines[new_symbol] = ComputeShader::define_value(new_value);
                  new_symbol[0] = '\0';
                  new_value[0] = '\0';
               }
               ImGui::EndTable();//DEFINES
            }
         }
         ImGui::End();
         UniformGui(cs->mShader, cs->mFilename.c_str());
         bool show_uniform_blocks = true;
         UniformBlockWindow(cs->mShader, cs->mFilename.c_str(), open[i]);
      }
      i++;
   }
}
    
