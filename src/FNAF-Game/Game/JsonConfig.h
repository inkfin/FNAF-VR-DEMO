#pragma once

#include "glm/gtc/matrix_transform.hpp"

#include "jsonLoader/json.hpp"
#include <fstream>

#include "Game/GlobalObjects.h"

using json = nlohmann::json;

struct characterConfig {
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 scale{};
    bool animated = false;
};

struct gameConfig {
    characterConfig freddy;
    characterConfig bunny;
};

class JsonConfig {
public:
    JsonConfig() = default;
    ~JsonConfig() = default;

    static json LoadJson(const std::string& path);

    static void WriteJson(const std::string& path, gameConfig game_config);

    static gameConfig ReadConfig(const std::string& path);

    static void RecordConfig(const std::string& path);

    static void LoadConfig(const std::string& path);
};