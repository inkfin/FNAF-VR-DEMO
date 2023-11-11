#pragma once
#include "Module.h"
#include "Shader.h"
#include "LoadMesh.h"
#include "StdUniforms.h"
#include "Timer.h"
#include <glm/glm.hpp>

namespace RendererGui
{
   void Menu();
   void DrawGui();
}

class Renderer:public Module
{
   public:
      void Init();
      void SetShader(Shader* pshader)  {pShader=pshader;}
      Shader* GetShader()  {return pShader;}
      StdUniforms::MaterialUbo& GetMaterial() {return mMaterial;}

      virtual void PreDraw(){}
      void Draw() override;
      virtual void PostDraw(){}
      void DrawGui() override;

   protected:
      Shader* pShader = nullptr;
      glm::mat4 mM;
      StdUniforms::MaterialUbo mMaterial;
      DoubleBufferedGpuTimer mTimer;

   friend void RendererGui::Menu();
   friend void RendererGui::DrawGui();
};



class MeshRenderer :public Renderer
{
   public:  
      void Draw() override;

      void SetMeshData(MeshData* pmesh) { pMeshData = pmesh; }
      void SetTexture(GLuint tex) { mTexture = tex; }

      MeshData* GetMeshData() {return pMeshData;}
      GLuint GetTexture()  {return mTexture;}
   protected:
      MeshData* pMeshData = nullptr;
      GLuint mTexture = -1;
      //glm::mat4 mM;

   public:
      static MeshRenderer GetDefault();
};

class SkyboxRenderer :public Renderer
{
   public:
      void Draw() override;
      void SetTexture(GLuint tex) { mTexture = tex; }
      GLuint GetTexture() { return mTexture; }
   protected:
      GLuint mTexture = -1;

   public:
      static SkyboxRenderer GetDefault();
};

class ParticleRenderer :public Renderer
{
public:
   void Draw() override;
   void DrawGui() override;
   void SetNumParticles(int n) { mNumParticles = n; }
   void SetBlendEnabled(bool blend) { mBlendEnabled = blend; }
   void SetBlendFunc(GLint src_factor, GLint dst_factor) {mSrcFactor=src_factor; mDstFactor = dst_factor;}
   void SetDepthMask(bool dmask) {mDepthMask = dmask;}

protected:
   int mNumParticles = 0;
   bool mBlendEnabled = false;
   GLint mSrcFactor = GL_SRC_ALPHA;
   GLint mDstFactor = GL_ONE;
   bool mDepthMask = true;

public:
   static ParticleRenderer GetAttrib2D();
};

class AttriblessParticleRenderer :public ParticleRenderer
{
   public:
      void Draw() override;

      static AttriblessParticleRenderer GetAttribless2D();
      static AttriblessParticleRenderer GetAttribless3D();
};

class HeightmapRenderer :public Renderer
{
   public:
      void Init() override;
      void Draw() override;
      void SetTexture(GLuint tex) { mTexture = tex; }

   protected:
      GLuint mTexture = -1;
       
      struct Uniforms
      {
         bool mWireframe = true;
         float mHeightScale = 1.0f;

      } mUniforms;
      struct UniformLocs
      {
         int mWireframe = -1;
         int mHeightScale = -1;
       
      } mUniformLocs;
      
   public:
      static HeightmapRenderer GetDefault();
};

class Texture2DRenderer : public Renderer
{
   public:
      void Draw() override;
      void SetTexture(GLuint tex) { mTexture = tex; }

   protected:
      GLuint mTexture = -1;

   public:
      static Texture2DRenderer GetDefault();
};

class Box2DRenderer : public Renderer
{
   public:
      void Draw() override;
      void SetNumBoxes(int n) { mNumBoxes = n; }
   protected:
      int mNumBoxes = 0;
   public:
      static Box2DRenderer GetDefault();
};

class Sphere2DRenderer : public Renderer
{
   public:
      void Draw() override;
      void SetNumSpheres(int n) { mNumSpheres = n; }
   protected:
      int mNumSpheres = 0;
   public:
      static Sphere2DRenderer GetDefault();
};

class ShadowmapRenderer: public Renderer
{
   public:
      
      void Init() override;
      void Draw() override;
      void DrawGui() override;

      void Clear();

      void SetShadowmapSize(glm::ivec2 size) {mShadowmapSize = size;}
      void SetRenderer(Renderer* prend) {pRenderer = prend;}
      void SetPolygonOffset(float factor, float units) {mPolygonOffset=glm::vec2(factor, units);}
      void SetM(const glm::mat4& M) {mShadowUniforms.M = M;}

      void SetLightProjection(StdUniforms::ProjectionParams proj_params);
      void SetLightLookAt(StdUniforms::LookAtParams look_params);
      void SetSceneUniforms(const StdUniforms::SceneUniforms& uniforms, const Buffer ubo);

      void PreDraw() override;
      void PostDraw() override;
      glm::mat4 GetShadowMatrix();
      GLuint GetShadowMap() {return mShadowmap;}

   protected:
      GLuint mFbo = -1;
      GLuint mShadowmap = -1;
      glm::ivec2 mShadowmapSize = glm::ivec2(1024);
      Renderer* pRenderer = nullptr;

      GLint mPrevViewport[4];
      GLint mPrevFbo = -1;
      GLint mPrevDrawBuffer[8];
      glm::vec2 mPolygonOffset = glm::vec2(0.0f, 0.0f);

      StdUniforms::ProjectionParams mProjParams = {1.5f, 0.1f, 1000.0f, 1.0f};
      StdUniforms::LookAtParams mLookAtParams = {glm::vec4(0.0f, 10.0f, 0.0f, 0.0f), glm::vec4(0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)};

      StdUniforms::SceneUniforms mShadowUniforms;
      Buffer mShadowUbo = Buffer(GL_UNIFORM_BUFFER, StdUniforms::UboBinding::scene);
      Buffer mSceneUbo;

      glm::mat4 mP_light;
      glm::mat4 mV_light;
};

class PickRenderer: public Renderer
{
   public:
      struct PickData
		{
         glm::ivec4 mouseover = glm::ivec4(-1);
         glm::ivec4 picked = glm::ivec4(-1);
         int pick_pass = 0;
			int pick_id = -1;
		};

      void Init() override;
      void DrawGui() override;
      void MouseCursor(glm::vec2 pos) override;
      void MouseButton(int button, int action, int mods, glm::vec2 pos) override;

      void PreDraw(int id, bool clear_pick_buffer);
      void PostDraw() override;
      void SetSize(glm::ivec2 size) {mSize = size;}
      void SetPickId(int id, bool pick_pass);
      glm::ivec2 GetMouseover(glm::ivec2 pos);
      
      PickData mPickData;

   protected:
      Buffer mUbo = Buffer(GL_UNIFORM_BUFFER);
		

      glm::ivec2 mSize;
      GLuint mFbo = -1;
      GLuint mRbo = -1;
      GLuint mPickTex = -1;

      GLint mPrevFbo = -1;
      GLint mPrevDrawBuffer[8];

      const int kPickOutLoc = 7;
		const int kPickUniformsBinding = 50;
};

