#pragma once

#include <glm/glm.hpp>
#include "Buffer.h"

namespace StdUniforms
{

   struct ProjectionParams
   {
      float mFov;
      float mNear;
      float mFar;
      float mAspect;
   };

   struct LookAtParams
   {
      glm::vec4 mPos;
      glm::vec4 mAt;
      glm::vec4 mUp;
   };

   struct SceneUniforms
   {
      //Scene Uniforms
      glm::vec4 clear_color = glm::vec4(0.0);
      glm::vec4 fog_color = glm::vec4(0.0);

      //Camera uniforms
      glm::mat4 P = glm::mat4(1.0);
      glm::mat4 V = glm::mat4(1.0);
      glm::mat4 PV = glm::mat4(1.0);	   //camera projection * view matrix
      glm::mat4 P_ortho = glm::mat4(1.0);
      glm::vec4 eye_w = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f);	//world-space eye position
      glm::ivec4 Viewport = glm::ivec4(-1);
      float ViewportAspect = 1.0;
      float pad0;
      float pad1;
      float pad2;

      //Frame Uniforms
      glm::ivec4 MouseButtonState = glm::ivec4(-1);
      glm::vec4 LmbClickAndDrag = glm::vec4(-1.0); //Left mouse button: xy=drag pos, zw = click pos
      glm::vec4 MmbClickAndDrag = glm::vec4(-1.0); //Middle mouse button: xy=drag pos, zw = click pos
      glm::vec4 RmbClickAndDrag = glm::vec4(-1.0); //Right mouse button: xy=drag pos, zw = click pos
      glm::vec4 MousePos = glm::vec4(-1.0); //xy = mouse pos, zw = delta
      glm::vec4 ScrollPos = glm::vec4(0.0f); //xy = scroll pos, zw = offset
      float Time = 0.0f;
      float DeltaTime = 0.0f;
      int Frame = 0;
      float pad5;

      //Object Uniforms
      glm::mat4 M;
   };

   struct LightUniforms
   {
      glm::vec4 La = (1.0f / 255.0f) * glm::vec4(74.0f, 52.0f, 45.0f, 1.0f);	//ambient light color
      glm::vec4 Ld = (1.0f / 255.0f) * glm::vec4(133.0f, 184.0f, 190.0f, 1.0f);	//diffuse light color
      glm::vec4 Ls = (1.0f / 255.0f) * glm::vec4(168.0f, 167.0f, 63.0f, 255.0f);	//specular light color
      glm::vec4 pos_w = glm::vec4(0.0f, 2.2, 1.0f, 1.0f); //world-space light position
      glm::vec4 quad_atten = glm::vec4(1.0, 0.0, 0.0, 0.0); //default to no attenutation
   };

   struct MaterialUniforms
   {
      glm::vec4 ka = glm::vec4(1.0f);	//ambient material color
      glm::vec4 kd = glm::vec4(1.0f);	//diffuse material color
      glm::vec4 ks = glm::vec4(1.0f);	//specular material color
      float shininess = 20.0f;         //specular exponent
   };

   struct MaterialUbo
   {
      void Init();
      void Bind();
      MaterialUniforms mMaterial;
      Buffer mUbo = Buffer(GL_UNIFORM_BUFFER);
   };

    //Locations for the uniforms which are not in uniform blocks
    namespace UniformLocs
    {
        extern int M; //model matrix
        extern int time;
        extern int pass;
    }

    namespace UboBinding
    {
        //These values come from the binding value specified in the shader block layout
        extern int scene;
        extern int light;
        extern int material;
    }
   
};
