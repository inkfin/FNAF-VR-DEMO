#include "Renderer.h"
#include "AttriblessRendering.h"
#include "LoadMesh.h"     
#include "LoadTexture.h" 
#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Defaults
{
   Shader mesh_shader("phong_vs.glsl", "phong_fs.glsl");
   MeshData mesh_data;
   const std::string mesh_name = "Amago0.obj";
   GLuint texture_id = -1;
   const std::string texture_name = "AmagoT.bmp";
   GLuint skybox_id = -1;
   const std::string skybox_name = "sorbin.png";

   Shader particle2d_shader("particle_vs.glsl", "particle_fs.glsl");
   Shader particle2d_attribless_shader("particle_attribless_vs.glsl", "particle_fs.glsl");
   Shader particle3d_shader("particle3d_vs.glsl", "particle3d_fs.glsl");
   Shader heightmap_shader("heightmap_vs.glsl", "heightmap_fs.glsl");
   Shader texture2d_shader("texture2D_vs.glsl", "texture2D_fs.glsl");
   Shader box2d_shader("box2d_vs.glsl", "box2d_fs.glsl");
   Shader sphere2d_shader("sphere2d_vs.glsl", "sphere2d_fs.glsl");
   Shader skybox_shader("skybox_vs.glsl", "skybox_fs.glsl");

   void InitMeshRendererDefaults()
   {
      if(Defaults::mesh_shader.GetShaderID()==-1)
      {
         Defaults::mesh_shader.Init();
      }
      if (Defaults::mesh_data.mScene == nullptr)
      {
         Defaults::mesh_data = LoadMesh(Defaults::mesh_name);
      }
      if (Defaults::texture_id == -1)
      {
         Defaults::texture_id = LoadTexture(Defaults::texture_name);
      }
      if (Defaults::skybox_id == -1)
      {
         Defaults::skybox_id = LoadSkybox(Defaults::skybox_name);
      }
   }
}

void Renderer::Init()
{
   if (pShader != nullptr)
   {
      if(pShader->GetShaderID() == -1)
      {
         pShader->Init();
      }
   }
   mMaterial.Init();
   mTimer.SetName(pShader->GetFilename(0));
}

void Renderer::Draw()
{
   mMaterial.Bind();
}

MeshRenderer MeshRenderer::GetDefault()
{
   Defaults::InitMeshRendererDefaults();
   MeshRenderer r;
   r.SetShader(&Defaults::mesh_shader);
   r.SetMeshData(&Defaults::mesh_data);
   r.SetTexture(Defaults::texture_id);
   r.Init();
   return r;
}

void MeshRenderer::Draw()
{
   if(mWantsDraw == false) return;
   mTimer.Restart();
   Renderer::Draw();

   pShader->UseProgram();

   if(mTexture != -1)
   {
      glBindTextureUnit(0, mTexture);
   }
   if(pMeshData != nullptr)
   {
      const int M_loc = 0;
      mM = glm::scale(glm::vec3(pMeshData->mScaleFactor));
      glProgramUniformMatrix4fv(pShader->GetShaderID(), M_loc, 1, false, glm::value_ptr(mM));
      glBindVertexArray(pMeshData->mVao);
      pMeshData->DrawMesh();
   }
   mTimer.Stop();
}

SkyboxRenderer SkyboxRenderer::GetDefault()
{
   Defaults::InitMeshRendererDefaults();
   SkyboxRenderer r;
   r.SetShader(&Defaults::skybox_shader);
   r.SetTexture(Defaults::skybox_id);
   r.Init();
   return r;
}

void SkyboxRenderer::Draw()
{
   if (mWantsDraw == false) return;
   mTimer.Restart();
   //Renderer::Draw(); //no material

   pShader->UseProgram();

   if (mTexture != -1)
   {
      glBindTextureUnit(0, mTexture);
   }

   glDepthMask(GL_FALSE);
   bind_attribless_vao();
   draw_attribless_cube();
   glDepthMask(GL_TRUE);
   mTimer.Stop();
}

void ParticleRenderer::Draw()
{
   if (mWantsDraw == false) return;

   mTimer.Restart();
   Renderer::Draw();

   if (mNumParticles <= 0) return;

   if (mBlendEnabled)
   {
      glEnable(GL_BLEND);
      glBlendFunc(mSrcFactor, mDstFactor);
      glDepthMask(GL_FALSE);
   }
   else
   {
      glDisable(GL_BLEND);
   }

   pShader->UseProgram();
   glDrawArrays(GL_POINTS, 0, mNumParticles);

   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);
   mTimer.Stop();
}

void ParticleRenderer::DrawGui()
{
   if (mGuiOpen == false) return;

   Renderer::DrawGui();
   if (ImGui::Begin(typeid(*this).name(), &mGuiOpen))
   {
      ImGui::Checkbox("Blend Enabled", &mBlendEnabled);
      ImGui::End();
   }
}

ParticleRenderer ParticleRenderer::GetAttrib2D()
{
   Defaults::InitMeshRendererDefaults();
   ParticleRenderer r;
   r.SetShader(&Defaults::particle2d_shader);
   r.Init();
   return r;
}

void AttriblessParticleRenderer::Draw()
{
   if (mWantsDraw == false) return;

   mTimer.Restart();
   Renderer::Draw();

   if(mNumParticles<=0) return;

   if (mBlendEnabled)
   {
      glEnable(GL_BLEND);
      glBlendFunc(mSrcFactor, mDstFactor);
      glDepthMask(GL_FALSE);
   }
   else
   {
      glDisable(GL_BLEND);
   }

   pShader->UseProgram();
   bind_attribless_vao();
   draw_attribless_particles(mNumParticles);

   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);
   mTimer.Stop();
}

AttriblessParticleRenderer AttriblessParticleRenderer::GetAttribless2D()
{
   Defaults::InitMeshRendererDefaults();
   AttriblessParticleRenderer r;
   r.SetShader(&Defaults::particle2d_attribless_shader);
   r.Init();
   return r;
}

AttriblessParticleRenderer AttriblessParticleRenderer::GetAttribless3D()
{
   Defaults::InitMeshRendererDefaults();
   AttriblessParticleRenderer r;
   r.SetShader(&Defaults::particle3d_shader);
   r.Init();
   return r;
}

void HeightmapRenderer::Init()
{
   Renderer::Init();

   mUniforms.mWireframe = false;
   Renderer::Init();
   mUniformLocs.mWireframe = glGetUniformLocation(pShader->GetShaderID(), "wireframe");
}

void HeightmapRenderer::Draw()
{
   if (mWantsDraw == false) return;
   mTimer.Restart();
   Renderer::Draw();

   pShader->UseProgram();
   glProgramUniform1i(pShader->GetShaderID(), mUniformLocs.mWireframe, int(mUniforms.mWireframe));

   int nx = 10, ny = 10;
   if (mTexture != -1)
   {
      const int level = 0;
      glGetTextureLevelParameteriv(mTexture, level, GL_TEXTURE_WIDTH, &nx);
      glGetTextureLevelParameteriv(mTexture, level, GL_TEXTURE_HEIGHT, &ny);
      glBindTextureUnit(0, mTexture);
   }
   else
   {
      glBindTextureUnit(0, 0);
   }
   if(mUniforms.mWireframe) 
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   }

   bind_attribless_vao();
   draw_attribless_triangle_grid(nx, ny);

   if (mUniforms.mWireframe) 
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }
   mTimer.Stop();
}

HeightmapRenderer HeightmapRenderer::GetDefault()
{
   Defaults::InitMeshRendererDefaults();
   HeightmapRenderer r;
   r.SetShader(&Defaults::heightmap_shader);
   r.Init();
   return r;
}


void Texture2DRenderer::Draw()
{
   if (mWantsDraw == false) return;

   mTimer.Restart();
   Renderer::Draw();

   pShader->UseProgram();
   glBindTextureUnit(0, mTexture);

   int vp[4];
   glGetIntegerv(GL_VIEWPORT, vp);
   float vp_aspect = float(vp[2])/float(vp[3]);
   const int vp_aspect_loc = 0;
   glProgramUniform1f(pShader->GetShaderID(), vp_aspect_loc, vp_aspect);

   glDepthMask(GL_FALSE);
   bind_attribless_vao();
   draw_attribless_quad();
   glDepthMask(GL_TRUE);
   mTimer.Stop();
}

Texture2DRenderer Texture2DRenderer::GetDefault()
{
   Defaults::InitMeshRendererDefaults();
   Texture2DRenderer r;
   r.SetShader(&Defaults::texture2d_shader);
   r.SetTexture(Defaults::texture_id);
   r.Init();
   return r;
}


void Box2DRenderer::Draw()
{
   if (mWantsDraw == false) return;
   if (mNumBoxes <= 0) return;

   mTimer.Restart();
   Renderer::Draw();
   pShader->UseProgram();
   bind_attribless_vao();
   glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, mNumBoxes);
   mTimer.Stop();
}

Box2DRenderer Box2DRenderer::GetDefault()
{
   Defaults::InitMeshRendererDefaults();
   Box2DRenderer r;
   r.SetShader(&Defaults::box2d_shader);
   r.Init();
   return r;
}

void Sphere2DRenderer::Draw()
{
   if (mWantsDraw == false) return;
   if (mNumSpheres <= 0) return;
   mTimer.Restart();
   Renderer::Draw();

   pShader->UseProgram();
   bind_attribless_vao();
   draw_attribless_particles(mNumSpheres);
   mTimer.Stop();
}

Sphere2DRenderer Sphere2DRenderer::GetDefault()
{
   Defaults::InitMeshRendererDefaults();
   Sphere2DRenderer r;
   r.SetShader(&Defaults::sphere2d_shader);
   r.Init();
   return r;
}

void Renderer::DrawGui()
{
   if (mGuiOpen == false) return;
   if (ImGui::Begin(typeid(*this).name(), &mGuiOpen))
   {
      if (pShader != nullptr)
      {
         ImGui::Text("Shader = %d", pShader->GetShaderID());
      }

      if(ImGui::CollapsingHeader("Flags"))
      {
         ImGui::Checkbox("Auto mode", &mAutoMode);
         ImGui::Checkbox("Enable Draw", &mWantsDraw);
      } 
   }
   ImGui::End();

   if (pShader != nullptr && pShader->GetShaderID() != -1)
   {
      pShader->DrawUniformGui(mGuiOpen);
      UniformBlockWindow(pShader->GetShaderID(), pShader->GetFilename(0).c_str(), mGuiOpen);
   }
}

void RendererGui::Menu()
{
   if (ImGui::BeginMenu("Renderers"))
   {
      int i = 0;
      for (Module* mod : Module::sAllModules())
      {
         Renderer* renderer = dynamic_cast<Renderer*>(mod);
         bool is_renderer = (renderer != nullptr);
         if (is_renderer)
         {
            ImGui::PushID(i);
            bool shader_valid = renderer->pShader != nullptr && renderer->pShader->GetShaderID() != -1;
            if (ImGui::BeginMenu(typeid(*mod).name(), shader_valid))
            {
               ImGui::BeginDisabled(renderer->mGuiOpen);
               if (ImGui::Button("Open window"))
               {
                  renderer->mGuiOpen = !renderer->mGuiOpen;
               }
               ImGui::EndDisabled();
               ImGui::Checkbox("Enable Draw", &renderer->mWantsDraw);
               ImGui::EndMenu();
            }
            ImGui::PopID();
         }
         i++;
      }
      ImGui::EndMenu();
   }
}

void RendererGui::DrawGui()
{
   for (Module* mod : Module::sAllModules())
   {
      Renderer* renderer = dynamic_cast<Renderer*>(mod);
      bool is_renderer = (renderer != nullptr);
      if (is_renderer && renderer->mAutoMode==false)
      {
         renderer->DrawGui();
      }
   }
}

void ShadowmapRenderer::Init()
{
   //Create depth texture
   glGenTextures(1, &mShadowmap);
   glBindTexture(GL_TEXTURE_2D, mShadowmap);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mShadowmapSize.x, mShadowmapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER  );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER  );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   
   //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS );
   float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
   glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 
   glBindTexture(GL_TEXTURE_2D, 0);

   //Create FBO
   glGenFramebuffers(1, &mFbo);
   glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowmap, 0);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   mShadowUbo.Init(sizeof(mShadowUniforms), &mShadowUniforms, GL_DYNAMIC_STORAGE_BIT);

   if(pRenderer != nullptr)
   {
      pRenderer->Init();
      pShader = pRenderer->GetShader();
   } 

   SetLightProjection(mProjParams);
   SetLightLookAt(mLookAtParams);
}

void ShadowmapRenderer::Draw()
{
   if (mWantsDraw == false) return;
   PreDraw();
   if (pRenderer != nullptr)
   {
      pRenderer->Draw();
   }
   PostDraw();
}

void ShadowmapRenderer::Clear()
{
   //Save current state
   glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mPrevFbo);
   glGetIntegerv(GL_VIEWPORT, mPrevViewport);

   for (int i = 0; i < 8; i++)
   {
      glGetIntegerv(GL_DRAW_BUFFER0 + i, &mPrevDrawBuffer[i]);
   }

   //Clear buffer
   glBindFramebuffer(GL_FRAMEBUFFER, mFbo); // render depth to shadowmap
   glDrawBuffer(GL_NONE);
   glViewport(0, 0, mShadowmapSize.x, mShadowmapSize.y);
   glClear(GL_DEPTH_BUFFER_BIT);

   //Restore state
   glBindFramebuffer(GL_FRAMEBUFFER, mPrevFbo);
   if (mPrevFbo == 0)
   {
      glDrawBuffers(1, (GLenum*)mPrevDrawBuffer);
   }
   else
   {
      glDrawBuffers(8, (GLenum*)mPrevDrawBuffer);
   }
   glViewport(mPrevViewport[0], mPrevViewport[1], mPrevViewport[2], mPrevViewport[3]);
}

void ShadowmapRenderer::PreDraw()
{
   //Save current state
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mPrevFbo);
    glGetIntegerv(GL_VIEWPORT, mPrevViewport);

    for (int i = 0; i < 8; i++)
    {
        glGetIntegerv(GL_DRAW_BUFFER0 + i, &mPrevDrawBuffer[i]);
    }

   //TODO: save prev polygon offset state

   //Setup shadowmapping
   glBindFramebuffer(GL_FRAMEBUFFER, mFbo); // render depth to shadowmap
   glDrawBuffer(GL_NONE);
   glViewport(0, 0, mShadowmapSize.x, mShadowmapSize.y);
   glClear(GL_DEPTH_BUFFER_BIT);

   //Setup P and V for light
   mShadowUniforms.P = mP_light;
   mShadowUniforms.V = mV_light;
   mShadowUniforms.PV = mP_light*mV_light;
   
   mShadowUniforms.Viewport = glm::ivec4(0, 0, mShadowmapSize.x, mShadowmapSize.y);
   mShadowUniforms.ViewportAspect = float(mShadowmapSize.x)/float(mShadowmapSize.y);
   mShadowUbo.BufferSubData(0, sizeof(mShadowUniforms), &mShadowUniforms);
   mShadowUbo.BindBufferBase();  

   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(mPolygonOffset[0], mPolygonOffset[1]); //No offset is being applied when params are 0.0, 0.0. Try changing these numbers to fix the shadow map "acne" problem
}

void ShadowmapRenderer::PostDraw()
{
   //Reset Scene Uniforms
   mSceneUbo.BindBufferBase();

   //Restore previous state
   glDisable(GL_POLYGON_OFFSET_FILL);
   glBindFramebuffer(GL_FRAMEBUFFER, mPrevFbo);
   if (mPrevFbo == 0)
   {
      glDrawBuffers(1, (GLenum*)mPrevDrawBuffer);
   }
   else
   {
      glDrawBuffers(8, (GLenum*)mPrevDrawBuffer);
   }
   glViewport(mPrevViewport[0], mPrevViewport[1], mPrevViewport[2], mPrevViewport[3]);
}

void ShadowmapRenderer::SetLightProjection(StdUniforms::ProjectionParams proj_params)
{
   mProjParams = proj_params;
   mP_light = glm::perspective(mProjParams.mFov, mProjParams.mAspect, mProjParams.mNear, mProjParams.mFar);
}

void ShadowmapRenderer::SetLightLookAt(StdUniforms::LookAtParams look_params)
{
   mLookAtParams = look_params;
   mV_light = glm::lookAt(glm::vec3(mLookAtParams.mPos), glm::vec3(mLookAtParams.mAt), glm::vec3(mLookAtParams.mUp));
}

void ShadowmapRenderer::SetSceneUniforms(const StdUniforms::SceneUniforms& uniforms, const Buffer ubo)
{
   mShadowUniforms = uniforms;
   mSceneUbo = ubo;
}


void ShadowmapRenderer::DrawGui()
{
   if (mGuiOpen == false) return;

   Renderer::DrawGui();
   if (ImGui::Begin(typeid(*this).name(), &mGuiOpen))
   {
      if (ImGui::CollapsingHeader("Light LookAt"))
      {
         bool pos = ImGui::SliderFloat3("Light Pos", &mLookAtParams.mPos.x, -10.0f, +10.0f);
         bool at = ImGui::SliderFloat3("Light At", &mLookAtParams.mAt.x, -10.0f, +10.0f);
         bool up = ImGui::SliderFloat3("Light Up", &mLookAtParams.mUp.x, -1.0f, 1.0f);
         if(pos || at || up)
         {
            SetLightLookAt(mLookAtParams);      
         }
      }

      if (ImGui::CollapsingHeader("Light Proj"))
      {
         bool fov = ImGui::SliderFloat("Fov", &mProjParams.mFov, 0.0f, 3.0f);
         bool n = ImGui::SliderFloat("Near", &mProjParams.mNear, 0.0f, 10.0f);
         bool f = ImGui::SliderFloat("Far", &mProjParams.mFar, 0.0f, 1000.0f);
         if(fov || n || f)
         {
            SetLightProjection(mProjParams);      
         }
      }

      if (ImGui::CollapsingHeader("Polygon Offset"))
      {
         ImGui::SliderFloat("Factor", &mPolygonOffset[0], 0.0f, 10.0f);
         ImGui::SliderFloat("Units", &mPolygonOffset[1], 0.0f, 1000.0f);
      }

      if (ImGui::CollapsingHeader("Shadowmap Preview"))
      {
         glBindTexture(GL_TEXTURE_2D, mShadowmap);
         glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
         ImGui::Image((void*)mShadowmap, ImVec2(128.0f, 128.0f), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0));
      }
      else
      {
         glBindTexture(GL_TEXTURE_2D, mShadowmap);
         glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
      }

      ImGui::End();
   }
}

glm::mat4 ShadowmapRenderer::GetShadowMatrix()
{
   const glm::mat4 S = glm::translate(glm::vec3(0.5f)) * glm::scale(glm::vec3(0.5f)); //scale and bias matrix
   glm::mat4 Shadow = S*mP_light*mV_light; //this matrix transforms world-space coordinates to shadow map texture coordinates
   return Shadow;
}


void PickRenderer::Init()
{
   if (mFbo != -1)
   {
      glDeleteFramebuffers(1, &mFbo);
      mFbo = -1;
   }

   if (mRbo != -1)
   {
      glDeleteRenderbuffers(1, &mRbo);
      mRbo = -1;
   }

   //Create depth RBO
   glCreateRenderbuffers(1, &mRbo);
   glNamedRenderbufferStorage(mRbo, GL_DEPTH_COMPONENT, mSize.x, mSize.y);

   //Create pick buffer
   glGenTextures(1, &mPickTex);
   glBindTexture(GL_TEXTURE_2D, mPickTex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mSize.x, mSize.y, 0, GL_RGBA, GL_FLOAT, 0);
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER  );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER  );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   
   
   //Create FBO
   glCreateFramebuffers(1, &mFbo);
   glNamedFramebufferTexture(mFbo, GL_COLOR_ATTACHMENT0, mPickTex, 0);
   glNamedFramebufferRenderbuffer(mFbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRbo);

   mUbo.mBinding = kPickUniformsBinding;
   mUbo.Init(sizeof(PickData), &mPickData, GL_DYNAMIC_STORAGE_BIT);

   mGuiOpen = true;
}

void PickRenderer::PreDraw(int id, bool clear_pick_buffer)
{
   SetPickId(id, true);
   mUbo.BindBufferBase();
   glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mPrevFbo);

   for (int i = 0; i < 8; i++)
   {
      glGetIntegerv(GL_DRAW_BUFFER0 + i, &mPrevDrawBuffer[i]);
   }

   glBindFramebuffer(GL_FRAMEBUFFER, mFbo); // render depth to pick buffer
   
   GLint buffers[8] = {GL_NONE};
   buffers[kPickOutLoc] = GL_COLOR_ATTACHMENT0;
   glDrawBuffers(8, (GLenum*)buffers);
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);

   if(clear_pick_buffer)
   {
      glm::vec4 clear_val = glm::vec4(glm::intBitsToFloat(-1));
      glClearNamedFramebufferfv(mFbo, GL_COLOR, kPickOutLoc, &clear_val.x);
      glClear(GL_DEPTH_BUFFER_BIT);
   }

}

glm::ivec2 PickRenderer::GetMouseover(glm::ivec2 pos)
{
   glm::vec4 b;
   glm::ivec2 coord(pos.x, mSize.y-1 -pos.y);
   if(coord.x<0 || coord.y<0 || coord.x >= mSize.x || coord.y >= mSize.y)
   {
      return glm::ivec2(-1);
   }
   //glGetTextureSubImage(mPickTex, 0, coord.x, coord.y, 0, 1, 1, 1, GL_RGBA, GL_FLOAT, sizeof(b), &b.x);

   glBindFramebuffer(GL_READ_FRAMEBUFFER, mFbo);
   glReadBuffer(GL_COLOR_ATTACHMENT0);
   glReadPixels(coord.x, coord.y, 1, 1, GL_RGBA, GL_FLOAT, &b.x);

   return glm::ivec2(glm::floatBitsToInt(b.x), glm::floatBitsToInt(b.y)); 
}

void PickRenderer::PostDraw()
{
   SetPickId(-1, false);
   glBindFramebuffer(GL_FRAMEBUFFER, mPrevFbo);
   if (mPrevFbo == 0)
   {
      glDrawBuffers(1, (GLenum*)mPrevDrawBuffer);
   }
   else
   {
      glDrawBuffers(8, (GLenum*)mPrevDrawBuffer);
   }
}

void PickRenderer::DrawGui()
{
   if (mGuiOpen == false) return;

   Renderer::DrawGui();
   if (ImGui::Begin(typeid(*this).name(), &mGuiOpen))
   {
      if (ImGui::CollapsingHeader("Pick Buffer Preview"))
      {
         ImGui::Image((void*)mPickTex, ImVec2(0.25f*mSize.x, 0.25f*mSize.y), ImVec2(0.0, 1.0), ImVec2(1.0, 0.0));
      }
      ImGui::Text("Mouseover ID: %d, %d", mPickData.mouseover.x, mPickData.mouseover.y);
   }
   ImGui::End();
}

void PickRenderer::MouseCursor(glm::vec2 pos)
{
   glm::ivec2 mouseover = GetMouseover(pos);
   mPickData.mouseover = glm::ivec4(mouseover, 0, 0);
   mUbo.BufferSubData(0, sizeof(PickData), &mPickData);
}

void PickRenderer::MouseButton(int button, int action, int mods, glm::vec2 pos)
{
   if(button == 0 && action == 1) //PRESS
   {
      mPickData.picked = glm::ivec4(GetMouseover(glm::ivec2(pos)), 0, 0);
   }
}

void PickRenderer::SetPickId(int id, bool pick_pass)
{
	mPickData.pick_id = id;
   mPickData.pick_pass = pick_pass;
	mUbo.BufferSubData(0, sizeof(PickData), &mPickData);
}