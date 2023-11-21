#include "Game.h"
#include "Game/GlobalObjects.h"

void Game::Init()
{
}

void Game::UpdateDynamicStep(float deltaTime)
{
	// Updated every render frame
        if (Scene::gFreddy.mStatus.moving) {
            const glm::vec2& p = Scene::gControllerState.trackpad_right;
            glm::vec3 direction;
            direction = glm::vec3(0.f);
            if (!(p.x == 0.f && p.y == 0.f)) {
                direction = glm::normalize(glm::vec3(p.x, 0.f, -p.y));
            }
            Scene::gFreddy.Move(deltaTime, direction);
        }
        if (Scene::gBunny.mStatus.moving) {
            const glm::vec2& p = Scene::gControllerState.trackpad_right;
            glm::vec3 direction;
            direction = glm::vec3(0.f);
            if (!(p.x == 0.f && p.y == 0.f)) {
                direction = glm::normalize(glm::vec3(p.x, 0.f, -p.y));
            }
            Scene::gBunny.Move(deltaTime, direction);
        }

        if (Scene::model_opt == 2) {
            const glm::vec2& p = Scene::gControllerState.trackpad_right;
            glm::vec3 direction;
            direction = glm::vec3(0.f);
            if (!(p.x == 0.f && p.y == 0.f)) {
                direction = glm::normalize(glm::vec3(p.x, 0.f, -p.y));
            }
            const float speed = 500.0f;
            Scene::LightData.light_w += glm::vec4(direction, 0.f) * speed * deltaTime;
        }

        Scene::camera->Move(deltaTime);
}

void Game::UpdateFixedStep()
{
	// Updated every 1/60 sec

}

void Game::Shutdown()
{
}
