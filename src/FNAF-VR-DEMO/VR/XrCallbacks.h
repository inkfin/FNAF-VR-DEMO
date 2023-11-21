#pragma once
#include <glm/glm.hpp>
#include <string>

namespace XrCallbacks
{
	namespace Hand
	{
		extern const int LEFT;// = 0;
		extern const int RIGHT;// = 1;
		extern const int COUNT;// = 2;
	};

	namespace View
	{
		extern const int LEFT;// = 0;
		extern const int RIGHT;// = 1;
		extern const int COUNT;// = 2;
	};

	//Inputs
	void TrackpadXYEvent(int hand, const glm::vec2& p);
	void TrackpadTouchEvent(int hand, bool click);
	void TrackpadClickEvent(int hand, bool click);
	void TriggerEvent(int hand, float squeeze);
	void TriggerClickEvent(int hand, bool click);
	void SqueezeClickEvent(int hand, bool click);
	void MenuClickEvent(int hand, bool click);

	void StagePoseEvent(const glm::mat4& M);
	void ControllerPoseEvent(int hand, const glm::quat& q, const glm::vec4& pos);
	void HmdPoseEvent(const glm::mat4& M);
	void ResizeViewEvent(int view, int size[2]);
	void ControllerActive(int hand, bool active);
	void ControllerProfileEvent(int hand, const std::string xr_path);
	
	void SetSwapChainFbo(unsigned int fbo);

	//Outputs
	bool GetVibrate(int hand);

}

