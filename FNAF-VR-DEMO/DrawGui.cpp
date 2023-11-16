
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include "DrawGui.h"


bool DrawGui::HideGui = false;

namespace DrawGui
{
   void Display(GLFWwindow* window)
   {
      if (HideGui == true) return;
      //Begin ImGui Frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      static bool show_debug_window = false;
      static bool show_capture_options = false;
      static bool show_imgui_test = false;
   
#pragma region menubar

      if (ImGui::BeginMainMenuBar())
      {
         if (ImGui::BeginMenu("File"))
         {
            
            ImGui::EndMenu();
         }
         if (ImGui::BeginMenu("Uniforms"))
         {
           
            ImGui::EndMenu();
         }

         if (ImGui::BeginMenu("Blit"))
         {
            
            ImGui::EndMenu();
         }

         if (ImGui::BeginMenu("Capture"))
         {
            if (ImGui::MenuItem("Show capture options", 0, show_capture_options))
            {
               show_capture_options = !show_capture_options;
            }
            ImGui::EndMenu();
         }
         if (ImGui::BeginMenu("Debug"))
         {
            if (ImGui::MenuItem("Show/Hide GUI", "F1", HideGui))
            {
               HideGui = !HideGui;
            }
            if (ImGui::MenuItem("Debug Menu", 0, show_debug_window))
            {
               show_debug_window = !show_debug_window;
            }
            
            if (ImGui::MenuItem("ImGui Test Window", 0, show_imgui_test))
            {
               show_imgui_test = !show_imgui_test;
            }
            
            ImGui::EndMenu();
         }
         ImGui::EndMainMenuBar();
      }
#pragma endregion 

      if(show_debug_window == true)
      {
         ImGui::Begin("Debug window", &show_debug_window);
         if (ImGui::Button("Quit"))
         {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
         }

         ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
         ImGui::End();
      }

      if (show_imgui_test)
      {
         ImGui::ShowDemoWindow(&show_imgui_test);
      }

      //End ImGui Frame
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   }
};