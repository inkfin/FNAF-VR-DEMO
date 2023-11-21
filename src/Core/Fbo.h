#pragma once
#include <windows.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "ImageTexture.h"


struct RenderFbo
{
   public:
      void Init(glm::ivec2 output_size, int n_color = 1); //n = number of color attachments to create
      void SetAttachment(int index, ImageTexture* pTex, int level = 0);
      void PushAttachment(ImageTexture* pTex, int level=0);
      void PreRender();
      void PostRender();

      std::vector<ImageTexture*> mOutputTextures;
      GLuint mFbo = -1;
      GLuint mRbo = -1;

      void Blit();
      GLenum mFilter = GL_NEAREST;

   private:
      glm::ivec4 mRestoreVp = glm::ivec4(-1);
      GLint mRestoreFbo = -1;
      GLint mRestoreDrawBuffer[8];
};

//Fbo strictly for blitting a texture to the screen
struct BlitFbo
{
   public:
      void Init();
      void SetInputTexture(ImageTexture img, int level = 0);
      void SetOutputSize(glm::ivec2 osize) { mOutputSize = osize; }
      void Blit(int viewport[4] = nullptr);
      void BlitRect(int rect[4], int viewport[4]);

      glm::ivec2 mInputSize = glm::ivec2(0);
      glm::ivec2 mOutputSize = glm::ivec2(0);
      int mInputLevel = 0;
      ImageTexture mInputTexture;
      GLuint mFbo = -1;
      GLenum mFilter = GL_NEAREST;

};


namespace BlitGui
{
   void Menu();
   void DrawGui();
   void SetBlitFbo(BlitFbo* pfbo);
}

namespace ThinWrapper
{
   struct RenderFbo
   {
      void Init(glm::ivec2 output_size, int n_color=1); //n = number of color attachments to create
      void PreRender();
      void PostRender();

      glm::ivec2 mOutputSize;
      std::vector<GLuint> mOutputTextures;
      GLuint mFbo = -1;
      GLuint mRbo = -1;
      int mNumAttachments = 0;

      void Blit();
      GLenum mFilter = GL_NEAREST;

      private:
         glm::ivec4 mRestoreVp;
   };

   //Fbo strictly for blitting a texture to the screen
   struct BlitFbo
   {
      void Init();
      void SetInputTexture(GLuint input_tex, int level = 0);
      void SetOutputSize(glm::ivec2 osize) {mOutputSize = osize;}
      void Blit();

      glm::ivec2 mInputSize;
      glm::ivec2 mOutputSize;
      GLuint mInputTexture = -1;
      GLuint mFbo = -1;
      GLenum mFilter = GL_NEAREST;

   };


   namespace BlitGui
   {
      void Menu();
      void DrawGui();
      void SetBlitFbo(BlitFbo* pfbo);
   }
};