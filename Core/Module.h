#pragma once

#include <glm/glm.hpp>
#include <list>

namespace ModuleGui
{
   void Menu();
   void DrawGui();
}

class Module
{
   public:
      Module(bool auto_mode = false);
      virtual ~Module();
      virtual void Init() {}
      virtual void Reinit() {}
      virtual void Draw() {}
      virtual void DrawGui();
      virtual void Compute() {}
      virtual void Animate(float t = -1.0f, float dt = -1.0f) {}
      virtual void Keyboard(int key, int scancode, int action, int mods) {}
      virtual void MouseCursor(glm::vec2 pos) {}
      virtual void MouseButton(int button, int action, int mods, glm::vec2 pos) {}

      bool GetAutoMode() {return mAutoMode;}
      void SetAutoMode(bool auto_mode) {mAutoMode = auto_mode;}

      bool GetWantsDraw()        {return mWantsDraw;}
      void SetWantsDraw(bool b)  {mWantsDraw = b;}

      void SetGuiOpen(bool open) {mGuiOpen = open;}

   protected:
      bool mAutoMode;
      bool mGuiOpen = false;
      bool mWantsDraw = true;
      bool mWantsDrawGui = true;
      bool mWantsAnimate = true;
      bool mWantsCompute = true;
      bool mWantsInput = true;
   public:
      static std::list<Module*>& sAllModules();

      static void sAutoInit();
      static void sAutoReinit();
      static void sAutoDraw();
      static void sAutoDrawGui();
      static void sAutoCompute();
      static void sAutoAnimate(float t, float dt);
      static void sAutoKeyboard(int key, int scancode, int action, int mods);
      static void sAutoMouseCursor(glm::vec2 pos);
      static void sAutoMouseButton(int button, int action, int mods, glm::vec2 pos);

   friend void ModuleGui::Menu();
   friend void ModuleGui::DrawGui();
};

