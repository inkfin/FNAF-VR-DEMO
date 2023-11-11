#include "LoadTexture.h"
#include "FreeImage.h"
#include <glm/glm.hpp>
#include <iostream>

static std::string TextureDir = "";
void SetTextureDir(std::string dir)
{
   TextureDir = dir;
}

std::string GetTextureDir() { return TextureDir; }

bool ValidTextureFilename(const std::string& fname0)
{
   std::string fname = TextureDir+fname0;
   FREE_IMAGE_FORMAT fmt = FreeImage_GetFileType(fname.c_str(), 0);
   if (fmt == FIF_UNKNOWN)
   {
      return false;
   }
   return true;
}

GLuint LoadTexture(const std::string& fname0)
{
   std::string fname = TextureDir + fname0;

   GLuint tex_id=-1;

   FIBITMAP* tempImg = FreeImage_Load(FreeImage_GetFileType(fname.c_str(), 0), fname.c_str());
   FIBITMAP* img = FreeImage_ConvertTo32Bits(tempImg);

   FreeImage_Unload(tempImg);
   if (tempImg == nullptr || img == nullptr)
   {
      FreeImage_Unload(img);
      std::cout << "FreeImage can't load image "<<fname<<std::endl;
      return tex_id;
   }

   GLuint w = FreeImage_GetWidth(img);
   GLuint h = FreeImage_GetHeight(img);
   GLuint scanW = FreeImage_GetPitch(img);

   GLubyte* byteImg = new GLubyte[h*scanW];
   FreeImage_ConvertToRawBits(byteImg, img, scanW, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, FALSE);
   FreeImage_Unload(img);
   
   glCreateTextures(GL_TEXTURE_2D, 1, &tex_id);
   int levels = 1+floor(log2(glm::max(w,h)));
   glTextureStorage2D(tex_id, levels, GL_RGBA8, w, h);
   glTextureSubImage2D(tex_id, 0, 0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, byteImg);
   glTextureParameterf(tex_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTextureParameterf(tex_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTextureParameterf(tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTextureParameterf(tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glGenerateTextureMipmap(tex_id);

   delete [] byteImg;

   return tex_id;
}

//Loads cubemap textures in the cross format.
//3 rows, 4 columns with square textures where the Xs are below:
// 0X00
// XXXX
// 0X00
GLuint LoadSkybox(const std::string& fname0)
{
   std::string fname = TextureDir + fname0;
   GLuint tex_id;

   FIBITMAP* tempImg = FreeImage_Load(FreeImage_GetFileType(fname.c_str(), 0), fname.c_str());
   FIBITMAP* img = FreeImage_ConvertTo32Bits(tempImg);

   FreeImage_Unload(tempImg);

   GLuint w = FreeImage_GetWidth(img);
   GLuint h = FreeImage_GetHeight(img);
   GLuint scanW = FreeImage_GetPitch(img);

   //TODO: handle case where h > w (vertical cross format)

   const int face_w = w/4;
   const int face_h = h/3;

   //+x, -x, +y, -y, +z, -z
   int left[6] =     {2*face_w,  0,          face_w,     face_w,     face_w,     3*face_w};
   int top[6] =      {face_h,    face_h,     0,          2*face_h,   face_h,     face_h};
   int right[6] =    {3*face_w,  face_w,     2*face_w,   2*face_w,   2*face_w,   4*face_w};
   int bottom[6] =   {2*face_h,  2*face_h,   face_h,     3*face_h,   2*face_h,   2*face_h};

   glGenTextures(1, &tex_id);
   glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
   glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   for (int i = 0; i<6; i++)
   {
      FIBITMAP* faceimg = FreeImage_Copy(img, left[i], top[i], right[i], bottom[i]);

      GLuint face_scanW = FreeImage_GetPitch(faceimg);

      GLubyte* byteImg = new GLubyte[face_h*face_scanW];
      FreeImage_ConvertToRawBits(byteImg, faceimg, face_scanW, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE); //last arg FALSE set origin of the texture to the lower left
      FreeImage_Unload(faceimg);

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, face_w, face_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, byteImg);

      delete [] byteImg;
   }
   FreeImage_Unload(img);

   glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
   glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
   return tex_id;
}

bool SaveTexture(const std::string& fname, GLuint tex)
{
   int w,h;
   glGetTextureLevelParameteriv(tex, 0, GL_TEXTURE_WIDTH, &w);
   glGetTextureLevelParameteriv(tex, 0, GL_TEXTURE_HEIGHT, &h);

   const int Bpp = 3;
   const int size = w*h*Bpp;
   GLubyte* byteImg = new GLubyte[size];

   const int level = 0;
   glPixelStorei(GL_PACK_ALIGNMENT, 1);
   glGetTextureImage(tex, level, GL_BGR, GL_UNSIGNED_BYTE, size, byteImg);
   FIBITMAP* image = FreeImage_ConvertFromRawBits(byteImg, w, h, Bpp*w, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
   bool success = FreeImage_Save(FIF_PNG, image, fname.c_str(), 0);
   return success;
}