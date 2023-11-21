#include "XrCallbacks.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Scene.h"

namespace XrCallbacks
{
	static bool Vibrate[2] = { false, false };

	namespace Hand
	{
		const int LEFT = 0;
		const int RIGHT = 1;
		const int COUNT = 2;
	};

	namespace View
	{
		const int LEFT = 0;
		const int RIGHT = 1;
		const int COUNT = 2;
	};

	glm::vec2 LastTrackpadXY[Hand::COUNT] = { glm::vec2(-1.0f) };
}

void XrCallbacks::HmdPoseEvent(const glm::mat4& M)
{
//	Scene::Mhmd = M * glm::translate(glm::vec3(0.0f, 0.0f, +1.0f));
}

void XrCallbacks::StagePoseEvent(const glm::mat4& M)
{
}

void XrCallbacks::ControllerPoseEvent(int hand, const glm::quat& q, const glm::vec4& pos)
{
//	Scene::Mcontroller[hand] = glm::translate(glm::vec3(pos)) * glm::mat4(q);
}

void XrCallbacks::TriggerEvent(int hand, float squeeze)
{
//	Scene::Trigger[hand] = squeeze;
}

void XrCallbacks::TriggerClickEvent(int hand, bool click)
{
    if (hand == 0) {
        Scene::gControllerState.triggerClick_left = click;
    } else if (hand == 1) {
        Scene::gControllerState.triggerClick_right = click;
    }
}

void XrCallbacks::SqueezeClickEvent(int hand, bool click)
{
    Scene::model_opt = (Scene::model_opt + 1) % 3;
    if (hand == 0) {
        Scene::gControllerState.squeezeClick_left = click;
    } else if (hand == 1) {
        Scene::gControllerState.squeezeClick_right = click;
    }
}

void XrCallbacks::MenuClickEvent(int hand, bool click)
{
    if (hand == 0) {
        Scene::gControllerState.menuClick_left = click;
    } else if (hand == 1) {
        Scene::gControllerState.menuClick_right = click;
    }
}

void XrCallbacks::TrackpadClickEvent(int hand, bool click)
{
    if (hand == 0) {
        Scene::gControllerState.trackpadClick_left = click;
    } else if (hand == 1) {
        Scene::gControllerState.trackpadClick_right = click;
    }
}

void XrCallbacks::TrackpadTouchEvent(int hand, bool click)
{
}

void XrCallbacks::TrackpadXYEvent(int hand, const glm::vec2& p)
{
    // 0 for left hand, 1 for right hand
    if (hand == 0)
    {
        //glm::vec3 delta_pos = glm::vec3(0.0f, -p.y, 0.f);
//        glm::vec3 delta_pos = glm::vec3(-p.x, 0.f, p.y);
//        Camera::Update(delta_pos);

        Scene::gControllerState.trackpad_left = p;
    }
//    else {
//        glm::vec3 delta_pos = glm::vec3(-p.x, 0.f, p.y);
//        Camera::Update(delta_pos);
//    }
    else {

        if (Scene::model_opt == 0) {
            Scene::gFreddy.mStatus.moving = true;
            Scene::gBunny.mStatus.moving = false;
        }
        else if (Scene::model_opt == 1) {
            Scene::gFreddy.mStatus.moving = false;
            Scene::gBunny.mStatus.moving = true;
        }
        else {
            Scene::gFreddy.mStatus.moving = false;
            Scene::gBunny.mStatus.moving = false;
        }

        Scene::gControllerState.trackpad_right = p;
    }
}

void XrCallbacks::ResizeViewEvent(int view, int size[2])
{

}

void XrCallbacks::SetSwapChainFbo(unsigned int fbo)
{
	
}

bool XrCallbacks::GetVibrate(int hand)
{
	bool vib = Vibrate[hand];
	Vibrate[hand] = false;
	return vib;
}

void XrCallbacks::ControllerProfileEvent(int hand, const std::string xr_path)
{
	//called when a controller is added or removed
}

void XrCallbacks::ControllerActive(int hand, bool active)
{
	//called while a controller is active
}
