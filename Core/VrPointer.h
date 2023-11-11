#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class VrPointer
{
	public:
		void Init();
		void SetPose(int hand, const glm::mat4& M);
		void Display();
		void SetEnabled(int hand, bool e) { mEnabled[hand] = e; }
		void SetColor(int hand, glm::vec3 color) { mColor[hand] = color; }
		void SetLength(int hand, float len) { mLength[hand] = len; }

	protected:
		float mLength[2] = {3.0f, 30.0f};
		glm::mat4 mMpose[2];
		bool mEnabled[2] = {false, false};
		glm::vec3 mColor[2] = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		GLuint mShader = -1;


};


