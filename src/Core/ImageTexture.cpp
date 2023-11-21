#include "ImageTexture.h"

ImageTexture::ImageTexture(GLenum target):mTarget(target)
{

}

void ImageTexture::CopyState(ImageTexture& src)
{
   mUnit = src.mUnit;
   mTarget = src.mTarget;
   mLevels = src.mLevels;
   mLayers = src.mLayers;
   mInternalFormat = src.mInternalFormat;

   mWrap = src.mWrap;
   mFilter = src.mFilter;
   mSize = src.mSize;
}

void ImageTexture::SetLevelsToMax()
{
   int max_dim = glm::max(mSize.x, glm::max(mSize.y, mSize.z));
   assert(max_dim > 0); //Set size before calling
   mLevels = 1 + int(glm::floor(glm::log2(float(max_dim))));
}

void ImageTexture::Init()
{
   if (mTexture != -1)
   {
      glDeleteTextures(1, &mTexture);
   }

   glCreateTextures(mTarget, 1, &mTexture);

   if (mTarget == GL_TEXTURE_1D)
   {
      glTextureStorage1D(mTexture, mLevels, mInternalFormat, mSize.x);
      glTextureParameteri(mTexture, GL_TEXTURE_WRAP_S, mWrap[0]);
   }
   if(mTarget==GL_TEXTURE_2D)
   {
      glTextureStorage2D(mTexture, mLevels, mInternalFormat, mSize.x, mSize.y);
      glTextureParameteri(mTexture, GL_TEXTURE_WRAP_S, mWrap[0]);
      glTextureParameteri(mTexture, GL_TEXTURE_WRAP_T, mWrap[1]);
   }
   if (mTarget == GL_TEXTURE_3D)
   {
      glTextureStorage3D(mTexture, mLevels, mInternalFormat, mSize.x, mSize.y, mSize.z);
      glTextureParameteri(mTexture, GL_TEXTURE_WRAP_S, mWrap[0]);
      glTextureParameteri(mTexture, GL_TEXTURE_WRAP_T, mWrap[1]);
      glTextureParameteri(mTexture, GL_TEXTURE_WRAP_R, mWrap[2]);
   }

   glTextureParameteri(mTexture, GL_TEXTURE_MIN_FILTER, mFilter[0]);
   glTextureParameteri(mTexture, GL_TEXTURE_MAG_FILTER, mFilter[1]);
}

void ImageTexture::Delete()
{
   if (mTexture != -1)
   {
      glDeleteTextures(1, &mTexture);
   }
   mTexture = -1;
}

void ImageTexture::BindImageTexture() const
{
   const int level = 0;
   bool layered = false;
   const int layer = 0;
   if (mTarget == GL_TEXTURE_3D)
   {
      layered = true;
   }
   glBindImageTexture(mUnit, mTexture, level, layered, layer, mAccess, mInternalFormat);
}

void ImageTexture::BindImageTexture(GLenum access) const
{
   mAccess = access;
   const int level = 0;
   bool layered = false;
   const int layer = 0;
   if (mTarget == GL_TEXTURE_3D)
   {
      layered = true;
   }
   glBindImageTexture(mUnit, mTexture, level, layered, layer, access, mInternalFormat);
}

void ImageTexture::BindImageTexture(GLuint unit, GLenum access) const
{
   mUnit = unit;
   BindImageTexture(access);
}

void ImageTexture::BindImageTexture(int level, bool layered, int layer, GLenum access) const
{
   mAccess = access;
   glBindImageTexture(mUnit, mTexture, level, layered, layer, access, mInternalFormat);
}

void ImageTexture::BindTextureUnit() const
{
   glBindTextureUnit(mUnit, mTexture);
}

/*
void ImageTexture::TextureParameter(GLenum pname, GLint param)
{
   glTextureParameteri(mTexture, pname, param);
}
*/

void ImageTexture::SetSize(glm::ivec3 size)
{
   if (mTexture != -1)
   {
      Resize(size);
      return;
   }
   mSize = size;
}

void ImageTexture::Resize(glm::ivec3 size)
{
   glm::ivec3 old_size = mSize;
   GLuint old_tex = mTexture;

   mSize = size;
   mTexture = -1;
   Init();

   if (mTarget == GL_TEXTURE_2D)
   {
      static GLuint fbo = -1;
      if (fbo == -1)
      {
         glGenFramebuffers(1, &fbo);
      }

      const int old_fbo = 0;
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, old_tex, 0);
      
      glReadBuffer(GL_COLOR_ATTACHMENT0);
      glm::ivec3 overlap = glm::min(old_size, mSize);
      glCopyTextureSubImage2D(mTexture, 0, 0, 0, 0, 0, overlap.x, overlap.y);
      glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
      glDeleteTextures(1, &old_tex);
   }
}

int GetMipmapLevels(GLuint tex)
{

   glm::ivec3 size;
   glGetTextureLevelParameteriv(tex, 0, GL_TEXTURE_WIDTH, &size.x);
   glGetTextureLevelParameteriv(tex, 0, GL_TEXTURE_HEIGHT, &size.y);
   glGetTextureLevelParameteriv(tex, 0, GL_TEXTURE_DEPTH, &size.z);
   int max_levels = 1 + (int)glm::floor(glm::log2((float)glm::max(size.x, glm::max(size.y, size.z))));
   int num_levels = 0;
   for (int i = 0; i < max_levels; i++)
   {
      int w;
      glGetTextureLevelParameteriv(tex, i, GL_TEXTURE_WIDTH, &w);
      if (w!=0)
      {
         num_levels++;
      }
      else
      {
         break;
      }
   }

   return num_levels;
}

ImageTexture CreateImage(GLuint tex)
{
   ImageTexture img;
   img.mTexture = tex;

   glGetTextureParameteriv(tex, GL_TEXTURE_TARGET, &img.mTarget);

   glGetTextureParameteriv(tex, GL_TEXTURE_VIEW_NUM_LAYERS, &img.mLayers);
   glGetTextureParameteriv(tex, GL_TEXTURE_VIEW_NUM_LEVELS, &img.mLevels);
   //img.mLevels = GetMipmapLevels(tex);

   glGetTextureParameteriv(tex, GL_TEXTURE_MIN_FILTER, &img.mFilter.x);
   glGetTextureParameteriv(tex, GL_TEXTURE_MAG_FILTER, &img.mFilter.y);

   glGetTextureParameteriv(tex, GL_TEXTURE_WRAP_S, &img.mWrap.x);
   glGetTextureParameteriv(tex, GL_TEXTURE_WRAP_T, &img.mWrap.y);
   glGetTextureParameteriv(tex, GL_TEXTURE_WRAP_R, &img.mWrap.y);

   glGetTextureLevelParameteriv(tex, 0, GL_TEXTURE_WIDTH, &img.mSize.x);
   glGetTextureLevelParameteriv(tex, 0, GL_TEXTURE_HEIGHT, &img.mSize.y);
   glGetTextureLevelParameteriv(tex, 0, GL_TEXTURE_DEPTH, &img.mSize.z);

   glGetTextureLevelParameteriv(tex, 0, GL_TEXTURE_INTERNAL_FORMAT, &img.mInternalFormat);

   return img;
}

#include <vector>

#include <iostream>
#include <fstream>
#include <cereal/archives/binary.hpp>
#include "zip.h"

#include "Timer.h"
#include "Serialize.hpp"

void progress_callback(zip* z, double prog, void* ud)
{
   float* pProgress = (float*)ud;
   *pProgress = 0.5f + 0.5f*prog;
}

bool SaveZip(ImageTexture& tex, std::string filename, float& progress)
{
   progress = 0.0f;
   const int level0 = 0;
   glm::ivec3 tex_size = tex.GetSize();
   const _int64 num_voxels = tex_size.x*tex_size.y*tex_size.z;
   const _int64 size = num_voxels*4;

   const int chunks = 256;
   _int64 chunk_size = size/chunks;
   std::vector<unsigned char> voxels[chunks];
   int slab_thickness = tex_size.z/chunks;

   ImageTexture chunk_tex;
   chunk_tex.CopyState(tex);
   chunk_tex.SetSize(glm::ivec3(tex_size.x, tex_size.y, slab_thickness));
   chunk_tex.Init();

   for(int i=0; i<chunks; i++)
   {
      voxels[i].resize(chunk_size);
      int slab_start = i*slab_thickness;
      void* data = voxels[i].data();
      //glGetTextureSubImage(tex.GetTexture(), level0, 0, 0, slab_start, tex_size.x, tex_size.y, slab_thickness, GL_RGBA, GL_UNSIGNED_BYTE, chunk_size, data);
      glCopyImageSubData(tex.GetTexture(), tex.GetTarget(), level0, 0, 0, slab_start,
                         chunk_tex.GetTexture(), chunk_tex.GetTarget(), level0, 0, 0, 0,
                         tex_size.x, tex_size.y, slab_thickness);
      glGetTextureImage(chunk_tex.GetTexture(), level0, GL_RGBA, GL_UNSIGNED_BYTE, chunk_size, data);
      progress = (float(i)/float(chunks-1))*0.25f;
   }
   chunk_tex.Delete();

   std::stringstream ss(std::ios::binary | std::ios::out); 
   cereal::BinaryOutputArchive oarchive(ss);
   serialize(oarchive, tex);
   std::string bin_tex = ss.str();

   int flags = ZIP_CREATE | ZIP_TRUNCATE; //create if doesn't exist, overwrite if does exist
   int open_err = 0;
   zip* z = zip_open(filename.c_str(), flags, &open_err);
   if(open_err != 0) return false;

   ZipBinary(z, bin_tex, "Texture Object");

   zip_source *s[chunks];
   
   for(int i=0; i<chunks; i++)
   {
      s[i] = zip_source_buffer(z, voxels[i].data(), chunk_size, 0);
      if(s[i]==nullptr) break;
      std::string nameinzip = std::string("voxels")+std::to_string(i)+".dat";
      int index = zip_file_add(z, nameinzip.c_str(), s[i], ZIP_FL_OVERWRITE);
      zip_set_file_compression(z, index, ZIP_CM_DEFLATE, 3);
      progress = 0.25f+float(i)/float(chunks-1)*0.25f;
   }

   zip_register_progress_callback_with_state(z, 0.05, progress_callback, nullptr, (void*)&progress);

   int close_err = zip_close(z);
   if(close_err<0) 
   {
      const char* error_msg = zip_strerror(z);
      std::cout << error_msg <<std::endl;
   }
   
   return close_err==0;
}

bool LoadZip(ImageTexture& tex, std::string filename, float& progress)
{
   progress = 0.0f;
   int open_error = 0;
   zip* z = zip_open(filename.c_str(), ZIP_RDONLY, &open_error);
      
   if(z==nullptr) return false;
   if(open_error != 0) return false;

   //Deserialize texture object
   std::string bin_tex;
   bool success = UnzipBinary(z, bin_tex, "Texture Object");
   std::stringstream ss(bin_tex);
   cereal::BinaryInputArchive iarchive(ss);
   serialize(iarchive, tex);

   glm::ivec3 tex_size = tex.GetSize();
   const int level0 = 0;
   const _int64 num_voxels = tex_size.x*tex_size.y*tex_size.z;
   const _int64 size = num_voxels*4;

   const int chunks = 256;
   _int64 chunk_size = size/chunks;
   int slab_thickness = tex_size.z/chunks;
   std::vector<unsigned char> voxel_chunk(chunk_size);

   ImageTexture chunk_tex;
   chunk_tex.CopyState(tex);
   chunk_tex.SetSize(glm::ivec3(tex_size.x, tex_size.y, slab_thickness));
   chunk_tex.Init();
   
   for(int i=0; i<chunks; i++)
   {
      int slab_start = i*slab_thickness;
      std::string nameinzip = std::string("voxels")+std::to_string(i)+".dat";
      zip_file_t *child_file = zip_fopen(z, nameinzip.c_str(), ZIP_FL_NOCASE);
      if(child_file==nullptr) break;

      int bytes_read = zip_fread(child_file, voxel_chunk.data(), chunk_size);
      if(bytes_read < 0) break;

      int close_err = zip_fclose(child_file);
      if(close_err != 0) break;

      //glTextureSubImage3D(tex.GetTexture(), level0, 0, 0, slab_start, tex_size.x, tex_size.y, slab_thickness, GL_RGBA, GL_UNSIGNED_BYTE, voxel_chunk.data());
      
      glTextureSubImage3D(chunk_tex.GetTexture(), level0, 0, 0, 0, tex_size.x, tex_size.y, slab_thickness, GL_RGBA, GL_UNSIGNED_BYTE, voxel_chunk.data());
      glCopyImageSubData(chunk_tex.GetTexture(), chunk_tex.GetTarget(), level0, 0, 0, 0,
                         tex.GetTexture(), tex.GetTarget(), level0, 0, 0, slab_start,
                         tex_size.x, tex_size.y, slab_thickness);
      progress = float(i)/float(chunks);
   }
   chunk_tex.Delete();

   int close_err = zip_close(z);
   if(close_err<0) 
   {
      const char* error_msg = zip_strerror(z);
      std::cout << error_msg <<std::endl;
   }

   return true;
}