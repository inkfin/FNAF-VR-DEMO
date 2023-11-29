//
// Created by boile on 2023/11/27.
//

#include "JsonConfig.h"

json JsonConfig::LoadJson(const std::string& path) {
    std::ifstream f(path);
    return json::parse(f);
}

void JsonConfig::WriteJson(const std::string& path, gameConfig game_config) {
    std::ofstream f(path);

    json config = {
        {"freddy_position_x", game_config.freddy.position.x},
        {"freddy_position_y", game_config.freddy.position.y},
        {"freddy_position_z", game_config.freddy.position.z},
        {"freddy_rotation_x", game_config.freddy.rotation.x},
        {"freddy_rotation_y", game_config.freddy.rotation.y},
        {"freddy_rotation_z", game_config.freddy.rotation.z},
        {"freddy_scale_x", game_config.freddy.scale.x},
        {"freddy_scale_y", game_config.freddy.scale.y},
        {"freddy_scale_z", game_config.freddy.scale.z},

        {"bunny_position_x", game_config.bunny.position.x},
        {"bunny_position_y", game_config.bunny.position.y},
        {"bunny_position_z", game_config.bunny.position.z},
        {"bunny_rotation_x", game_config.bunny.rotation.x},
        {"bunny_rotation_y", game_config.bunny.rotation.y},
        {"bunny_rotation_z", game_config.bunny.rotation.z},
        {"bunny_scale_x", game_config.bunny.scale.x},
        {"bunny_scale_y", game_config.bunny.scale.y},
        {"bunny_scale_z", game_config.bunny.scale.z},
    };

    f << config.dump(4);
}

gameConfig JsonConfig::ReadConfig(const std::string& path) {
    std::ifstream f(path);
    json config = json::parse(f);

    gameConfig game_config;

    game_config.freddy.position = glm::vec3(config["freddy_position_x"].template get<float>(), config["freddy_position_y"].template get<float>(), config["freddy_position_z"].template get<float>());
    game_config.freddy.rotation = glm::vec3(config["freddy_rotation_x"].template get<float>(), config["freddy_rotation_y"].template get<float>(), config["freddy_rotation_z"].template get<float>());
    game_config.freddy.scale = glm::vec3(config["freddy_scale_x"].template get<float>(), config["freddy_scale_y"].template get<float>(), config["freddy_scale_z"].template get<float>());

    game_config.bunny.position = glm::vec3(config["bunny_position_x"].template get<float>(), config["bunny_position_y"].template get<float>(), config["bunny_position_z"].template get<float>());
    game_config.bunny.rotation = glm::vec3(config["bunny_rotation_x"].template get<float>(), config["bunny_rotation_y"].template get<float>(), config["bunny_rotation_z"].template get<float>());
    game_config.bunny.scale = glm::vec3(config["bunny_scale_x"].template get<float>(), config["bunny_scale_y"].template get<float>(), config["bunny_scale_z"].template get<float>());

    return game_config;
}

void JsonConfig::recordConfig(const std::string& path) {
    gameConfig config;

    config.freddy.position = Scene::gFreddy.mMesh->mTranslation;
    config.freddy.rotation = Scene::gFreddy.mMesh->mRotation;
    config.freddy.scale = Scene::gFreddy.mMesh->mScale;

    config.bunny.position = Scene::gBunny.mMesh->mTranslation;
    config.bunny.rotation = Scene::gBunny.mMesh->mRotation;
    config.bunny.scale = Scene::gBunny.mMesh->mScale;

    JsonConfig::WriteJson(path, config);
}
