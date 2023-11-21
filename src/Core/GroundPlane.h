#pragma once

#include <GL/glew.h>

class GroundPlane
{
	public:
		void Init();
		void Display();

	protected:
		GLuint mShader;

};


#include "Module.h"
#include "Shader.h"
class GroundPlaneModule :public Module
{
	public:
		GroundPlaneModule();
		void Init() override;
		void Draw() override;
		Shader& GetShader() { return mShader; }
	protected:
		Shader mShader;
};