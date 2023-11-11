#ifndef __LOADTEXTURE_H__
#define __LOADTEXTURE_H__

#include <string>
#include <windows.h>
#include "GL/glew.h"
#include "GL/gl.h"

GLuint LoadTexture(const std::string& fname);
bool SaveTexture(const std::string& fname, GLuint tex);
GLuint LoadSkybox(const std::string& fname);
bool ValidTextureFilename(const std::string& fname);
void SetTextureDir(std::string dir);
std::string GetTextureDir();

#endif