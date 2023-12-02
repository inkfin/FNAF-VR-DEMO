#include "Game.h"
#include "Game/GlobalObjects.h"

void Game::Init()
{
}

void Game::GeneraCase(float deltaTime) {
        static int freddy_stop_flag = 0;
        if (Scene::gControllerState.squeezeClick_left && freddy_stop_flag == 0) {
            freddy_stop_flag = 1;
            if  (Scene::gFreddy.mStatus.shock_level <= SHOCK_LEVEL_MAX) {
                Scene::gFreddy.mStatus.shock_level++;
                float rate = (float)Scene::gFreddy.mStatus.shock_level / 5.f;
                Scene::gFreddy.mStatus.speed = 0.6f * (1.f + rate);
                // Scene::gFreddy.mStatus.anime_speed = 1.f / (1.f + rate);
            }
        }
        else if (Scene::gControllerState.squeezeClick_left && freddy_stop_flag == 1) {
            // do nothing
        }
        else if (!Scene::gControllerState.squeezeClick_left && freddy_stop_flag == 1) {
            freddy_stop_flag = 0;
        }
        else {
            Scene::gFreddy.Move(deltaTime, glm::vec3(0.f, 0.f, 1.f));
        }

        if (Scene::gFreddy.mTranslation.z >= -10.f) {
            Scene::is_game_over = true;
            return;
        }

        static int bunny_show_flag = 0;
        static int bunny_hold_count = 0;
        int random_number = rand() % 10;
        if (bunny_hold_count < 120) {
            bunny_hold_count++;
        }
        else if (random_number < 7) {
            bunny_show_flag = 0;
            JsonConfig::LoadBunnyConfig("Configs/bunny_hide_pos.json");
            bunny_hold_count = 0;
        }
        else {
            bunny_show_flag = 1;
            JsonConfig::LoadBunnyConfig("Configs/bunny_show_pos.json");
            bunny_hold_count = 0;
        }

        static int bunny_death_count = 0;
        if (bunny_show_flag == 1 && Scene::gControllerState.squeezeClick_left) {
            bunny_death_count++;
            if (bunny_death_count > 30) {
                Scene::is_game_over = true;
            }
        }
        else {
            bunny_death_count = 0;
        }
}

void Game::DeadCase() {
        static int light_off_count = 0;
        if (light_off_count == 0) {
                // light off
        }
        else if (light_off_count < 150) {
                light_off_count++;
        }
        else {
                // light on
                JsonConfig::LoadConfig("Configs/death_pos.json");
                // load death scene, like "Death" with a line of words said "Press any key to restart"
                // Moniter the key pressed event, if pressed, restart the game, reset the game status
                // if (Condition) {
                //      Scene::is_game_over = false;
                //      Scene::is_game_started = false;
                //      light_off_count = 0;
                // }
        }
}

void Game::UpdateDynamicStep(float deltaTime)
{
        if (!Scene::is_game_started) {
            // Show the start scene
            return;
        }
        else if (Scene::is_game_over) {
            DeadCase();
        }
        else {
            GeneraCase(deltaTime);
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
