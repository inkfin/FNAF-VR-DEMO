#include "Fbo.h"


void RenderFbo::Init(glm::ivec2 output_size, int n)
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

   //Create an FBO 
   glCreateFramebuffers(1, &mFbo);

   //Create depth RBO
   glCreateRenderbuffers(1, &mRbo);
   glNamedRenderbufferStorage(mRbo, GL_DEPTH_COMPONENT, output_size.x, output_size.y);

   //attach depth renderbuffer to depth attachment
   glNamedFramebufferRenderbuffer(mFbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRbo);

   //Create texture objects and set initial wrapping and filtering state
   mOutputTextures.resize(0);
   const int levels = 1;
   for (int i = 0; i < n; i++)
   {
      ImageTexture* pTex = new ImageTexture(GL_TEXTURE_2D);
      pTex->SetSize(glm::ivec3(output_size, 1));
      pTex->SetLevels(levels);
      pTex->SetInternalFormat(GL_RGBA32F);
      pTex->SetFilter(glm::ivec2(GL_NEAREST));
      pTex->SetWrap(glm::ivec3(GL_CLAMP_TO_EDGE));
      pTex->Init();
      mOutputTextures.push_back(pTex);

      //attach the texture we just created to the FBO
      glNamedFramebufferTexture(mFbo, GL_COLOR_ATTACHMENT0 + i, pTex->GetTexture(), 0);
   }
}

void RenderFbo::PushAttachment(ImageTexture* pTex, int level)
{
   const GLuint attachment = GL_COLOR_ATTACHMENT0+mOutputTextures.size();
   mOutputTextures.push_back(pTex);
   glNamedFramebufferTexture(mFbo, attachment, pTex->GetTexture(), level);
}

void RenderFbo::SetAttachment(int index, ImageTexture* pTex, int level)
{
   const GLuint attachment = GL_COLOR_ATTACHMENT0 + index;
   mOutputTextures[index]=pTex;
   glNamedFramebufferTexture(mFbo, attachment, pTex->GetTexture(), level);
}

void RenderFbo::PreRender()
{
   glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mRestoreFbo);
   for (int i = 0; i < 8; i++)
   {
      glGetIntegerv(GL_DRAW_BUFFER0 + i, &mRestoreDrawBuffer[i]);
   }

   glBindFramebuffer(GL_FRAMEBUFFER, mFbo); // Render to FBO.
   const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
                           GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
                           GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
                           GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15 };

   glDrawBuffers(mOutputTextures.size(), buffers);
   glGetIntegerv(GL_VIEWPORT, &mRestoreVp.x);

   glm::ivec2 size = mOutputTextures[0]->GetSize();
   glViewport(0, 0, size.x, size.y);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderFbo::PostRender()
{
   //resume drawing to the back buffer
   glViewport(mRestoreVp[0], mRestoreVp[1], mRestoreVp[2], mRestoreVp[3]);

   glBindFramebuffer(GL_FRAMEBUFFER, mRestoreFbo);
   if (mRestoreFbo == 0)
   {
      glDrawBuffers(1, (GLenum*)mRestoreDrawBuffer);
   }
   else
   {
      glDrawBuffers(8, (GLenum*)mRestoreDrawBuffer);
   }
}

void RenderFbo::Blit()
{
   //blit fbo texture to screen
   const GLuint default_framebuffer = 0;
   glm::ivec3 size = mOutputTextures[0]->GetSize();
   glNamedFramebufferReadBuffer(mFbo, GL_COLOR_ATTACHMENT0);
   glBlitNamedFramebuffer(mFbo, default_framebuffer, 0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, mFilter);
}

void BlitFbo::Init()
{
   if (mFbo != -1)
   {
      glDeleteFramebuffers(1, &mFbo);
      mFbo = -1;
   }
   //Create an FBO for blitting to the screen
   glCreateFramebuffers(1, &mFbo);

}

void BlitFbo::SetInputTexture(ImageTexture img, int level)
{
   mInputTexture = img;
   mInputLevel = level;
   mInputSize = img.GetSize()>> mInputLevel;
   glNamedFramebufferTexture(mFbo, GL_COLOR_ATTACHMENT0, mInputTexture.GetTexture(), level);
}

void BlitFbo::Blit(int viewport[4])
{
   //blit fbo texture to screen
   const GLuint default_framebuffer = 0;
   glNamedFramebufferReadBuffer(mFbo, GL_COLOR_ATTACHMENT0);
   if(viewport == nullptr)
   {
      glBlitNamedFramebuffer(mFbo, default_framebuffer, 0, 0, mInputSize.x, mInputSize.y, 0, 0, mOutputSize.x, mOutputSize.y, GL_COLOR_BUFFER_BIT, mFilter);
   }
   else
   {
      float input_aspect = float(mInputSize.x)/float(mInputSize.y);
      float output_aspect = float(viewport[2]) / float(viewport[3]);
      glm::ivec4 output_rect(viewport[0], viewport[1], viewport[2], viewport[3]);
      if (input_aspect > output_aspect)
      {
         output_rect[3] = viewport[2]/input_aspect;
         output_rect[1] = (viewport[3]-output_rect[3])*0.5;
      }
      else
      {
         output_rect[2] = viewport[3]*input_aspect;
         output_rect[0] = (viewport[2]-output_rect[2])*0.5;
      }

      glBlitNamedFramebuffer(mFbo, default_framebuffer, 
         0, 0, mInputSize.x, mInputSize.y, 
         output_rect[0], output_rect[1], output_rect[2] + output_rect[0], output_rect[3] + output_rect[1],
         GL_COLOR_BUFFER_BIT, mFilter);
   }
}

void BlitFbo::BlitRect(int rect[4], int viewport[4]) //(x0, y0, width, height)
{
   const GLuint default_framebuffer = 0;
   glNamedFramebufferReadBuffer(mFbo, GL_COLOR_ATTACHMENT0);

   float input_aspect = float(rect[2]) / float(rect[3]);
   float output_aspect = float(viewport[2]) / float(viewport[3]);
   glm::ivec4 output_rect(viewport[0], viewport[1], viewport[2]>>mInputLevel, viewport[3]>>mInputLevel);
   if (input_aspect > output_aspect)
   {
      output_rect[3] = viewport[2] / input_aspect;
      output_rect[1] = (viewport[3] - output_rect[3]) * 0.5;
   }
   else
   {
      output_rect[2] = viewport[3] * input_aspect;
      output_rect[0] = (viewport[2] - output_rect[2]) * 0.5;
   }

   glBlitNamedFramebuffer(mFbo, default_framebuffer, 
      rect[0], rect[1], rect[2]+rect[0], rect[3]+rect[1], 
      output_rect[0], output_rect[1], output_rect[2]+ output_rect[0], output_rect[3]+ output_rect[1],
      GL_COLOR_BUFFER_BIT, mFilter);
}

#include "imgui.h"

namespace BlitGui
{
   bool ShowBlitPicker = false;
   BlitFbo* pBlit = nullptr;
}

void BlitGui::SetBlitFbo(BlitFbo* pfbo)
{
   BlitGui::pBlit = pfbo;
}

void BlitGui::Menu()
{
   if (pBlit == nullptr) return;
   if (ImGui::BeginMenu("Blit"))
   {
      //ImGui::MenuItem("Blit Picker", "", &ShowBlitPicker);
      if (ImGui::BeginMenu("Blit Picker"))
      {
         ImGui::BeginDisabled(ShowBlitPicker);
         if (ImGui::Button("Open window"))
         {
            ShowBlitPicker = !ShowBlitPicker;
         }
         ImGui::EndDisabled();

         GLuint fbo = pBlit->mFbo;
         if (glIsFramebuffer(fbo))
         {
            static int tex_id = -1;
            static int level = 0;
            if (tex_id == -1)
            {
               glGetNamedFramebufferAttachmentParameteriv(fbo, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tex_id);
            }

            static bool first = true;
            static bool texture_valid = false;
            static bool level_valid = false;
            static bool target_valid = false;
            static glm::ivec2 level_size(1);
            static int target = -1;

            bool id_changed = ImGui::SliderInt("Tex ID", &tex_id, 0, 32);
            bool level_changed = ImGui::SliderInt("Tex Level", &level, 0, 12);

            if (id_changed || level_changed || first)
            {  
               first = false;
               texture_valid = glIsTexture(tex_id);
               if (texture_valid)
               {
                  glGetTextureParameteriv(tex_id, GL_TEXTURE_TARGET, &target);
                  target_valid = (target == GL_TEXTURE_2D);
               }

               if (texture_valid && target_valid)
               {
                  glGetTextureLevelParameteriv(tex_id, level, GL_TEXTURE_WIDTH, &level_size.x);
                  glGetTextureLevelParameteriv(tex_id, level, GL_TEXTURE_HEIGHT, &level_size.y);
                  level_valid = (level_size.x != 0);
               }

               if (texture_valid && target_valid && level_valid)
               {
                  ImageTexture img = CreateImage(tex_id);
                  pBlit->SetInputTexture(img, level);
               }
            }
            if (level_valid == false)
            {
               ImGui::Text("Not a valid mipmap level");
            }
            if (texture_valid == false || target_valid == false)
            {
               ImGui::Text("Not a valid 2D texture id");
            }
            if (texture_valid && target_valid && level_valid)
            {
               ImGui::Image((void*)tex_id, ImVec2(128, 128));
            }

         }
         ImGui::EndMenu();
      }
      ImGui::EndMenu();
   }
}

void BlitGui::DrawGui()
{
   if (pBlit == nullptr) return;
   if (ShowBlitPicker == true)
   {
      if (ImGui::Begin("BlitPicker", &ShowBlitPicker))
      {
         GLuint fbo = pBlit->mFbo;
         if (glIsFramebuffer(fbo))
         {
            static int tex_id = -1;
            static int level = 0;
            if (tex_id == -1)
            {
               glGetNamedFramebufferAttachmentParameteriv(fbo, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tex_id);
            }

            static bool texture_valid = false;
            static bool level_valid = false;
            static bool target_valid = false;
            static glm::ivec2 level_size(1);
            static int target = -1;

            bool id_changed = ImGui::SliderInt("Tex ID", &tex_id, 0, 32);
            bool level_changed = ImGui::SliderInt("Tex Level", &level, 0, 12);

            if (id_changed || level_changed)
            {
               texture_valid = glIsTexture(tex_id);
               if (texture_valid)
               {
                  glGetTextureParameteriv(tex_id, GL_TEXTURE_TARGET, &target);
                  target_valid = (target == GL_TEXTURE_2D);
               }

               if (texture_valid && target_valid)
               {
                  glGetTextureLevelParameteriv(tex_id, level, GL_TEXTURE_WIDTH, &level_size.x);
                  glGetTextureLevelParameteriv(tex_id, level, GL_TEXTURE_HEIGHT, &level_size.y);
                  level_valid = (level_size.x != 0);
               }

               if (texture_valid && target_valid && level_valid)
               {
                  ImageTexture img = CreateImage(tex_id);
                  pBlit->SetInputTexture(img, level);
               }
            }
            if (level_valid == false)
            {
               ImGui::Text("Not a valid mipmap level");
            }
            if (texture_valid == false || target_valid == false)
            {
               ImGui::Text("Not a valid 2D texture id");
            }
            if (texture_valid && target_valid && level_valid)
            {
               ImGui::Image((void*)tex_id, ImVec2(level_size.x, level_size.y));
            }

         }
         ImGui::End();
      }
   }
}

























namespace ThinWrapper
{
void RenderFbo::Init(glm::ivec2 output_size, int n)
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

   if (mNumAttachments > 0)
   {
      glDeleteTextures(mNumAttachments, mOutputTextures.data());
      mNumAttachments = 0;
   }

   mOutputSize = output_size;
   mNumAttachments = n;
   mOutputTextures.resize(mNumAttachments);

   //Create an FBO 
   glCreateFramebuffers(1, &mFbo);
   
   //Create depth RBO
   glCreateRenderbuffers(1, &mRbo);
   glNamedRenderbufferStorage(mRbo, GL_DEPTH_COMPONENT, mOutputSize.x, mOutputSize.y);

   //attach depth renderbuffer to depth attachment
   glNamedFramebufferRenderbuffer(mFbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRbo);

   //Create texture objects and set initial wrapping and filtering state
   const int levels = 1;
   glCreateTextures(GL_TEXTURE_2D, mNumAttachments, mOutputTextures.data());
   for(int i=0; i< mNumAttachments; i++)
   {
      glTextureStorage2D(mOutputTextures[i], levels, GL_RGBA32F, mOutputSize.x, mOutputSize.y);
      glTextureParameteri(mOutputTextures[i], GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTextureParameteri(mOutputTextures[i], GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTextureParameteri(mOutputTextures[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(mOutputTextures[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      //attach the texture we just created to the FBO
      glNamedFramebufferTexture(mFbo, GL_COLOR_ATTACHMENT0+i, mOutputTextures[i], 0);
   }
}

void RenderFbo::PreRender()
{
   glBindFramebuffer(GL_FRAMEBUFFER, mFbo); // Render to FBO.
   const GLenum buffers [] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, 
                           GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, 
                           GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, 
                           GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15};

   glDrawBuffers(mNumAttachments, buffers);
   //glDrawBuffer(GL_COLOR_ATTACHMENT0);
   glGetIntegerv(GL_VIEWPORT, &mRestoreVp.x);
   glViewport(0, 0, mOutputSize.x, mOutputSize.y);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderFbo::PostRender()
{
   //resume drawing to the back buffer
   glViewport(mRestoreVp[0], mRestoreVp[1], mRestoreVp[2], mRestoreVp[3]);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glDrawBuffer(GL_BACK);
}

void RenderFbo::Blit()
{
   //blit fbo texture to screen
   const GLuint default_framebuffer = 0;
   glNamedFramebufferReadBuffer(mFbo, GL_COLOR_ATTACHMENT0);
   glBlitNamedFramebuffer(mFbo, default_framebuffer, 0, 0, mOutputSize.x, mOutputSize.y, 0, 0, mOutputSize.x, mOutputSize.y, GL_COLOR_BUFFER_BIT, mFilter);
}

void BlitFbo::Init()
{
   if (mFbo != -1)
   {
      glDeleteFramebuffers(1, &mFbo);
      mFbo = -1;
   }
   //Create an FBO for blitting to the screen
   glCreateFramebuffers(1, &mFbo);

}

void BlitFbo::SetInputTexture(GLuint input_tex, int level)
{
   mInputTexture = input_tex;
   glGetTextureLevelParameteriv(input_tex, level, GL_TEXTURE_WIDTH, &mInputSize.x);
   glGetTextureLevelParameteriv(input_tex, level, GL_TEXTURE_HEIGHT, &mInputSize.y);
   glNamedFramebufferTexture(mFbo, GL_COLOR_ATTACHMENT0, mInputTexture, level);
}

void BlitFbo::Blit()
{
   //blit fbo texture to screen
   const GLuint default_framebuffer = 0;
   glNamedFramebufferReadBuffer(mFbo, GL_COLOR_ATTACHMENT0);
   glBlitNamedFramebuffer(mFbo, default_framebuffer, 0, 0, mInputSize.x, mInputSize.y, 0, 0, mOutputSize.x, mOutputSize.y, GL_COLOR_BUFFER_BIT, mFilter);
}

#include "imgui.h"

namespace BlitGui
{
   bool ShowBlitPicker = false;
   BlitFbo* pBlit = nullptr;
}

void BlitGui::SetBlitFbo(BlitFbo* pfbo)
{
   BlitGui::pBlit = pfbo;
}

void BlitGui::Menu()
{
   if(pBlit==nullptr) return;
   if (ImGui::BeginMenu("Blit"))
   {
      ImGui::MenuItem("Blit Picker", "", &ShowBlitPicker);
      ImGui::EndMenu();
   }
}

void BlitGui::DrawGui()
{
   if (pBlit == nullptr) return;
   if (ShowBlitPicker == true)
   {
      if(ImGui::Begin("BlitPicker", &ShowBlitPicker))
      {
         GLuint fbo = pBlit->mFbo;
         if(glIsFramebuffer(fbo))
         {
            static int tex_id = -1;
            static int level = 0;
            if(tex_id==-1)
            {
               glGetNamedFramebufferAttachmentParameteriv(fbo, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tex_id);
            }
            
            static bool texture_valid = false;
            static bool level_valid = false;
            static bool target_valid = false;
            static glm::ivec2 level_size(1);
            static int target = -1;

            bool id_changed = ImGui::SliderInt("Tex ID", &tex_id, 0, 32);
            bool level_changed = ImGui::SliderInt("Tex Level", &level, 0, 12);
            
            if (id_changed || level_changed)
            {
               texture_valid = glIsTexture(tex_id);
               if(texture_valid) 
               {
                  glGetTextureParameteriv(tex_id, GL_TEXTURE_TARGET, &target);
                  target_valid = (target==GL_TEXTURE_2D);
               }

               if (texture_valid && target_valid)
               {
                  glGetTextureLevelParameteriv(tex_id, level, GL_TEXTURE_WIDTH, &level_size.x);
                  glGetTextureLevelParameteriv(tex_id, level, GL_TEXTURE_HEIGHT, &level_size.y);
                  level_valid = (level_size.x != 0);
               }
              
               if (texture_valid && target_valid && level_valid)
               {
                  pBlit->SetInputTexture(tex_id, level);
               }
            }
            if (level_valid == false)
            {
               ImGui::Text("Not a valid mipmap level");
            }
            if (texture_valid == false || target_valid == false)
            {
               ImGui::Text("Not a valid 2D texture id");
            }
            if (texture_valid && target_valid && level_valid)
            {
               ImGui::Image((void*)tex_id, ImVec2(level_size.x, level_size.y));
            }

         }
         ImGui::End();
      }
   }
}

};