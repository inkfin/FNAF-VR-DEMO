#include "Module.h"

//std::list<Module*> Module::sAllModules; //no, construct on first use instead

Module::Module(bool auto_mode):mAutoMode(auto_mode)
{
   sAllModules().push_back(this);
}

Module::~Module()
{
   sAllModules().remove(this);
}

std::list<Module*>& Module::sAllModules()
{
   static std::list<Module*> all_modules;
   return all_modules;
}

void Module::sAutoInit()
{
   for(Module* p: sAllModules())
   {
      if (p->mAutoMode) p->Init();
   }
}

void Module::sAutoReinit()
{
   for (Module* p : sAllModules())
   {
      if (p->mAutoMode) p->Reinit();
   }
}

void Module::sAutoDraw()
{
   for(Module* p: sAllModules())
   {
      if (p->mAutoMode) p->Draw();
   }
}

void Module::sAutoDrawGui()
{
   for(Module* p: sAllModules())
   {
      if (p->mAutoMode) p->DrawGui();
   }
}

void Module::sAutoCompute()
{
   for(Module* p: sAllModules())
   {
      if (p->mAutoMode) p->Compute();
   }
}

void Module::sAutoAnimate(float t, float dt)
{
   for(Module* p: sAllModules())
   {
      if (p->mAutoMode) p->Animate(t, dt);
   }
}

void Module::sAutoKeyboard(int key, int scancode, int action, int mods)
{
   for(Module* p: sAllModules())
   {
      if (p->mAutoMode) p->Keyboard(key, scancode, action, mods);
   }
}

void Module::sAutoMouseCursor(glm::vec2 pos)
{
   for(Module* p: sAllModules())
   {
      if (p->mAutoMode) p->MouseCursor(pos);
   }
}

void Module::sAutoMouseButton(int button, int action, int mods, glm::vec2 pos)
{
   for(Module* p: sAllModules())
   {
      if (p->mAutoMode) p->MouseButton(button, action, mods, pos);
   }
}



#include "imgui.h"
#include <typeinfo> 

void Module::DrawGui()
{
   if (mGuiOpen == true)
   {
      ImGui::Begin(typeid(*this).name(), &mGuiOpen);

      if (ImGui::Button("Reinit"))
      {
         Reinit();
      }
      if(ImGui::CollapsingHeader("Flags"))
      {
         ImGui::Checkbox("Auto mode", &mAutoMode);
         ImGui::Checkbox("Enable Draw", &mWantsDraw);
         ImGui::Checkbox("Enable DrawGui", &mWantsDrawGui);
         ImGui::Checkbox("Enable Animate", &mWantsAnimate);
         ImGui::Checkbox("Enable Compute", &mWantsCompute);
         ImGui::Checkbox("Enable Input", &mWantsInput);
      }
      ImGui::End();
   }
}



#include "Renderer.h"
#include "ComputeTests.h"


void ModuleGui::Menu()
{
   if (ImGui::BeginMenu("Modules"))
   {
      for (Module* mod : Module::sAllModules())
      {
         bool is_renderer = (dynamic_cast<Renderer*>(mod) != nullptr);
         bool is_compute_test = (dynamic_cast<ComputeTest*>(mod) != nullptr);
         if(!is_renderer && !is_compute_test)
         {
            if (ImGui::BeginMenu(typeid(*mod).name()))
            {
               ImGui::BeginDisabled(mod->mGuiOpen);
               if (ImGui::Button("Open window"))
               {
                  mod->mGuiOpen = !mod->mGuiOpen;
               }
               ImGui::EndDisabled();
               if (ImGui::Button("Reinit"))
               {
                  mod->Reinit();
               }
               ImGui::Checkbox("Auto mode", &mod->mAutoMode);
               ImGui::Checkbox("Enable Draw", &mod->mWantsDraw);
               ImGui::Checkbox("Enable DrawGui", &mod->mWantsDrawGui);
               ImGui::Checkbox("Enable Animate", &mod->mWantsAnimate);
               ImGui::Checkbox("Enable Compute", &mod->mWantsCompute);
               ImGui::Checkbox("Enable Input", &mod->mWantsInput);
               ImGui::EndMenu();
            }
         }
      }
      ImGui::EndMenu();
   }
}

void ModuleGui::DrawGui()
{
   for (Module* mod : Module::sAllModules())
   {
      bool is_renderer = (dynamic_cast<Renderer*>(mod) != nullptr);
      bool is_compute_test = (dynamic_cast<ComputeTest*>(mod) != nullptr);
      if (!is_renderer && !is_compute_test)
      {
         if (mod->mGuiOpen == true && mod->mAutoMode==false)
         {
            mod->DrawGui();
         }
      }
   }
}

