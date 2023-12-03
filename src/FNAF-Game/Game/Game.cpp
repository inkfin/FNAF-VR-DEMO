#include "Game.h"
#include "Game/GlobalObjects.h"

void Game::Init()
{
    JsonConfig::LoadConfig(R"(Configs\vr_ini_config.json)");
    JsonConfig::LoadGameLoopConfig("Configs/game_logic_config.json");
    LightManager::use_flash_light = false;
    Scene::is_game_over = false;
    Scene::game_result = false;
    Scene::gFreddy.mStatus.active = false;
    Scene::gFreddy.mStatus.shock_level = SHOCK_LEVEL_MIN;
    Scene::gBunny.mStatus.active = false;
    Scene::EndTitleShow = false;

    bool freddy_stop_flag = 0;
    bool bunny_show_flag = 1;
    int bunny_hold_count = 0;
    int game_time_count = 0;
}

void Game::InitGameStart() {
    LightManager::use_flash_light = false;
    Scene::is_game_over = false;
    Scene::game_result = false;
    Scene::gFreddy.mStatus.active = true;
    Scene::gFreddy.mStatus.shock_level = SHOCK_LEVEL_MIN;
    Scene::gBunny.mStatus.active = true;
    Scene::EndTitleShow = false;

    bool freddy_stop_flag = 0;
    bool bunny_show_flag = 1;
    int bunny_hold_count = 0;
    int game_time_count = 0;

}

void Game::DeadLight(int mode)
{
    if (mode == 0) {
        // All dark
        LightManager::use_flash_light = false;
    } else {
        // Flash light
    }
}

void Game::StartScene()
{
    Init();
}

void Game::EndScene()
{
}

void Game::GeneraCase(float deltaTime)
{
    if ((Scene::gControllerState.squeezeClick_left || Scene::key_flash_light) && Scene::freddy_stop_flag == 0) {
        Scene::freddy_stop_flag = 1;
        Scene::gFreddy.mStatus.active = false;
        if (Scene::gFreddy.mStatus.shock_level <= SHOCK_LEVEL_MAX) {
            Scene::gFreddy.mStatus.shock_level++;
            Scene::rate = (float)Scene::gFreddy.mStatus.shock_level / Scene::game_loop_config.speed_increase_rate;
            Scene::gFreddy.mStatus.speed = Scene::freddy_ini_speed * (1.f + Scene::rate);
        }
        LightManager::use_flash_light = true;
    } else if ((Scene::gControllerState.squeezeClick_left || Scene::key_flash_light) && Scene::freddy_stop_flag == 1) {
        // do nothing
    } else if ((!Scene::gControllerState.squeezeClick_left || !Scene::key_flash_light) && Scene::freddy_stop_flag == 1) {
        Scene::freddy_stop_flag = 0;
        Scene::gFreddy.mStatus.active = true;
        LightManager::use_flash_light = false;
    } else {
        Scene::gFreddy.Move(deltaTime, glm::vec3(0.f, 0.f, 1.f));
    }

    if (Scene::gFreddy.mTranslation.z >= Scene::game_loop_config.freddy_death_distance) {
        Scene::is_game_over = true;
        Scene::game_result = false;
        return;
    }

    int random_number = rand() % 10;
    if (Scene::bunny_hold_count < Scene::game_loop_config.bunny_show_time) {
        Scene::bunny_hold_count++;
    } else {
        Scene::bunny_hold_count = 0;
        if (random_number < Scene::game_loop_config.bunny_show_rate) {
            Scene::bunny_show_flag = 0;
            JsonConfig::LoadBunnyLocation("Configs/bunny_hide_pos.json");
        } else {
            Scene::bunny_show_flag = 1;
            JsonConfig::LoadBunnyLocation("Configs/bunny_show_pos.json");
        }
    }

    static int bunny_death_count = 0;
    if (Scene::bunny_show_flag == 1 && (Scene::gControllerState.squeezeClick_left || Scene::key_flash_light)) {
        bunny_death_count++;
        if (bunny_death_count > Scene::game_loop_config.bunny_react_time) {
            Scene::is_game_over = true;
            Scene::game_result = false;
            bunny_death_count = 0;
            return;
        }
    } else {
        bunny_death_count = 0;
    }
}

void Game::DeadCase()
{
    static int light_off_count = 0;
    if (light_off_count == 0) {
        // Use light manager to turn off the light
        DeadLight(0);
    } else if (light_off_count > 60) {
        Scene::EndTitleShow = true;
        DeadLight(1);
        EndScene();
        JsonConfig::LoadConfig("Configs/death_pos.json");
        Scene::gFreddy.mStatus.active = true;

        static int dead_scene_count = 0;
        if ((Scene::gControllerState.squeezeClick_left || Scene::key_flash_light)) {
            dead_scene_count++;
            if (dead_scene_count > 60) {
                dead_scene_count = 0;
                Scene::is_game_over = false;
                Scene::is_game_started = false;
                light_off_count = 0;
                return;
            }
        } else {
            dead_scene_count = 0;
        }
    }
    light_off_count++;
}

void Game::WinCase()
{
    Scene::gFreddy.mStatus.active = false;
    Scene::gBunny.mStatus.active = false;
    JsonConfig::LoadBunnyLocation("Configs/bunny_show_pos.json");
    LightManager::use_flash_light = ~LightManager::use_flash_light;
    static int win_scene_count = 0;
    if ((Scene::gControllerState.squeezeClick_left || Scene::key_flash_light)) {
        win_scene_count++;
        if (win_scene_count > 60) {
            win_scene_count = 0;
            Scene::is_game_over = false;
            Scene::is_game_started = false;
            return;
        }
    } else {
        win_scene_count = 0;
    }
}

void Game::UpdateDynamicStep(float deltaTime)
{
    static int start_scene_count = 0;
    if (!Scene::is_game_started) {
        StartScene();
        if ((Scene::gControllerState.squeezeClick_left || Scene::key_flash_light)) {
            start_scene_count++;
            if (start_scene_count > 60) {
                start_scene_count = 0;
                Scene::is_game_started = true;
                InitGameStart();
                return;
            }
        } else {
            start_scene_count = 0;
        }
        return;
    }

    if (Scene::is_game_over) {
        if (Scene::game_result == 0) {
            DeadCase();
        } else {
            WinCase();
        }
    } else {
        GeneraCase(deltaTime);
    }

    if (Scene::game_time_count < Scene::game_loop_config.game_time) {
        Scene::game_time_count++;
    } else {
        Scene::game_time_count = 0;
        Scene::is_game_over = true;
        Scene::game_result = true;
        return;
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
