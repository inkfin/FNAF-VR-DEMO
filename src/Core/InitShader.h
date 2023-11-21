#ifndef __INITSHADER_H__
#define __INITSHADER_H__

#include <windows.h>
#include <string>
#include <GL/GL.h>

GLuint InitShader(const std::string& computeShaderFile);
GLuint InitShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
GLuint InitShader(const std::string& vertexShaderFile, const std::string& geometryShader, const std::string& fragmentShaderFile);
GLuint InitShader(const std::string& vertexShaderFile, const std::string& tessControlShader, const std::string& tessEvalShader, const std::string& fragmentShaderFile);
GLuint InitShader(const std::string& vertexShaderFile, const std::string& tessControlShader, const std::string& tessEvalShader, const std::string& geometryShader, const std::string& fragmentShaderFile);

void SetShaderDir(const std::string& dir);
const std::string GetShaderDir();

void SetCodeInjection(const std::string& inj);
void ClearCodeInjection();
const std::string GetCodeInjection();

#endif