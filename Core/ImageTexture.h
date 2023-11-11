#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class ImageTexture
{
   public:
      
      ImageTexture(GLenum target = GL_TEXTURE_2D);
      void Init();
      void Delete();
      void BindImageTexture() const;
      void BindImageTexture(GLenum access) const;
      void BindImageTexture(GLuint unit, GLenum access) const;
      void BindImageTexture(int level, bool layered, int layer, GLenum access) const;
   
      void BindTextureUnit() const;

      void CopyState(ImageTexture& src); //only state, not the texture ID

      void SetTarget(GLint target)  {mTarget = target;}
      GLint GetTarget() const {return mTarget;}

      void SetInternalFormat(GLint iformat) {mInternalFormat = iformat;}
      GLint GetInternalFormat() const {return mInternalFormat;}

      void SetLevels(GLint levels) {mLevels = levels;}
      void SetLevelsToMax();
      int GetLevels() const { return mLevels; }

      void SetFilter(glm::ivec2 filter) {mFilter = filter;}
      void SetWrap(glm::ivec3 wrap) {mWrap = wrap;}

      void SetUnit(GLuint unit) const {mUnit = unit;}
      GLuint GetUnit() const {return mUnit;}

      void SetSize(glm::ivec3 size);
      glm::ivec3 GetSize() const {return mSize;}
      void Resize(glm::ivec3 size);

      GLuint GetTexture() const {return mTexture;}

      void SetAccess(GLuint access) { mAccess = access; }
      GLuint GetAccess() const { return mAccess; }

   private:
      GLuint mTexture = -1;
      GLint mTarget = -1;
      GLint mLevels = 1;
      GLint mLayers = 1;
      GLint mInternalFormat = GL_RGBA32F;

      mutable GLuint mUnit = -1;
      mutable GLuint mAccess = GL_READ_ONLY;

      glm::ivec3 mWrap = glm::ivec3(GL_REPEAT, GL_REPEAT, GL_REPEAT);
      glm::ivec2 mFilter = glm::ivec2(GL_LINEAR, GL_NEAREST);
      glm::ivec3 mSize = glm::ivec3(0);

   friend ImageTexture CreateImage(GLuint tex);
   friend bool Save(ImageTexture tex, std::string filename);

   template<class Archive>
   friend void serialize(Archive & archive, ImageTexture &tex);
   
};

ImageTexture CreateImage(GLuint tex);
bool SaveCompressed(ImageTexture tex, std::string filename);
bool SaveZip(ImageTexture& tex, std::string filename, float& progress);
bool LoadZip(ImageTexture& tex, std::string filename, float& progress);