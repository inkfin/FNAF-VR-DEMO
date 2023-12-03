#pragma once

#include "glm/gtc/matrix_transform.hpp"

#include "jsonLoader/json.hpp"
#include <fstream>

#include "Game/GlobalObjects.h"
#include "Objects/LightManager.h"

using json = nlohmann::json;

class JsonConfig {
public:
    JsonConfig() = default;
    ~JsonConfig() = default;

    static json LoadJson(const std::string& path);

    static void WritePawnJson(const std::string& path);
    static void WriteLightPosition(const std::string& path);
    static void WriteLightColor(const std::string& path);

    static void LoadConfig(const std::string& path);

    static void LoadFreddyLocation(const std::string& path);
    static void LoadBunnyLocation(const std::string& path);

    static void LoadLightColorConfig(const std::string& path);
    static void LoadLightPositionConfig(const std::string& path);

    static void LoadGameLoopConfig(const std::string& path);
};