//
// Created by boile on 2023/11/27.
//

#include "JsonConfig.h"

#define SET_VEC3_CONFIG(config_name, cpp_name)\
{ #config_name"_x", cpp_name.x },\
{ #config_name"_y", cpp_name.y },\
{ #config_name"_z", cpp_name.z }

#define GET_VEC3_CONFIG(config_object, config_name, cpp_name)\
cpp_name = glm::vec3(config_object[#config_name"_x"].get<float>(), config_object[#config_name"_y"].get<float>(), config_object[#config_name"_z"].get<float>())

#define GET_VEC3_CONFIG_STR(config_object, config_str, cpp_name)\
cpp_name = glm::vec3(config_object[config_str+"_x"].get<float>(), config_object[config_str+"_y"].get<float>(), config_object[config_str+"_z"].get<float>())

#define GET_FLOAT_CONFIG(config_object, config_name, cpp_name)\
cpp_name = config_object[config_name].get<float>()

#define GET_INT_CONFIG(config_object, config_name, cpp_name)\
cpp_name = config_object[config_name].get<int>()

json JsonConfig::LoadJson(const std::string& path)
{
    std::ifstream f(path);
    return json::parse(f);
}

static void WriteJson(const std::string& path, json config) {
    std::ofstream f(path);
    f << config.dump(4);
}

void JsonConfig::WritePawnJson(const std::string& path)
{
    using namespace Scene;
    json config = {
        SET_VEC3_CONFIG(freddy_position, gFreddy.mTranslation),
        SET_VEC3_CONFIG(freddy_rotation, gFreddy.mRotation),
        SET_VEC3_CONFIG(freddy_scale, gFreddy.mScale),

        SET_VEC3_CONFIG(bunny_position, gBunny.mTranslation),
        SET_VEC3_CONFIG(bunny_rotation, gBunny.mRotation),
        SET_VEC3_CONFIG(bunny_scale, gBunny.mScale),
    };

    WriteJson(path, config);
}


void JsonConfig::WriteLightPosition(const std::string& path) {
    using namespace Scene;
    json config = {
        SET_VEC3_CONFIG(point_light0_position, LightManager::pointLightData[0].position),
        SET_VEC3_CONFIG(point_light1_position, LightManager::pointLightData[1].position),
        SET_VEC3_CONFIG(point_light2_position, LightManager::pointLightData[2].position),
        SET_VEC3_CONFIG(point_light3_position, LightManager::pointLightData[3].position),

        SET_VEC3_CONFIG(bunny_position, gBunny.mTranslation),
        SET_VEC3_CONFIG(bunny_rotation, gBunny.mRotation),
        SET_VEC3_CONFIG(bunny_scale, gBunny.mScale),
    };

    WriteJson(path, config);
}

void JsonConfig::LoadConfig(const std::string& path)
{
    using namespace Scene;
    std::ifstream f(path);
    json config = json::parse(f);

    GET_VEC3_CONFIG(config, freddy_position, gFreddy.mTranslation);
    GET_VEC3_CONFIG(config, freddy_rotation, gFreddy.mRotation);
    GET_VEC3_CONFIG(config, freddy_scale, gFreddy.mScale);

    GET_VEC3_CONFIG(config, bunny_position, gBunny.mTranslation);
    GET_VEC3_CONFIG(config, bunny_rotation, gBunny.mRotation);
    GET_VEC3_CONFIG(config, bunny_scale, gBunny.mScale);
}

void JsonConfig::LoadFreddyLocation(const std::string& path)
{
    using namespace Scene;
    std::ifstream f(path);
    json config = json::parse(f);

    GET_VEC3_CONFIG(config, freddy_position, gFreddy.mTranslation);
}

void JsonConfig::LoadBunnyLocation(const std::string& path)
{
    using namespace Scene;
    std::ifstream f(path);
    json config = json::parse(f);

    GET_VEC3_CONFIG(config, bunny_position, gBunny.mTranslation);
}

void JsonConfig::LoadLightColorConfig(const std::string& path)
{
    std::ifstream f(path);
    json config = json::parse(f);

    // point light config
    for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
        const std::string light_name = "point_light" + std::to_string(i);
        LightManager::PointLightUniforms& light = LightManager::pointLightData[i];

        GET_VEC3_CONFIG_STR(config, light_name+"_La", light.La);
        GET_VEC3_CONFIG_STR(config, light_name+"_Ld", light.La);
        GET_VEC3_CONFIG_STR(config, light_name+"_Ls", light.La);
        GET_FLOAT_CONFIG(config, light_name+"_constant", light.constant);
        GET_FLOAT_CONFIG(config, light_name+"_linear", light.linear);
        GET_FLOAT_CONFIG(config, light_name+"_quadratic", light.quadratic);
    }

    // spotlight config
    {
        LightManager::SpotLightUniforms& light = LightManager::spotLightData;
        GET_VEC3_CONFIG(config, spotlight_La, light.La);
        GET_VEC3_CONFIG(config, spotlight_Ld, light.Ld);
        GET_VEC3_CONFIG(config, spotlight_Ls, light.Ls);
        GET_FLOAT_CONFIG(config, "spotlight_constant", light.constant);
        GET_FLOAT_CONFIG(config, "spotlight_linear", light.linear);
        GET_FLOAT_CONFIG(config, "spotlight_quadratic", light.quadratic);
    }
}


void JsonConfig::LoadLightPositionConfig(const std::string& path)
{
    std::ifstream f(path);
    json config = json::parse(f);

    // point light config
    for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
        const std::string light_name = "point_light" + std::to_string(i);
        LightManager::PointLightUniforms& light = LightManager::pointLightData[i];

        GET_VEC3_CONFIG_STR(config, light_name+"_position", light.position);
    }

    // spotlight config
    {
        LightManager::SpotLightUniforms& light = LightManager::spotLightData;
        GET_VEC3_CONFIG(config, spotlight_position, light.position);
        GET_VEC3_CONFIG(config, spotlight_direction, light.direction);
        GET_FLOAT_CONFIG(config, "spotlight_cutOff", light.cutOff);
    }
}

void JsonConfig::LoadGameLoopConfig(const std::string& path) {
    using namespace Scene;
    std::ifstream f(path);
    json config = json::parse(f);

    GET_INT_CONFIG(config, "GameTime", game_loop_config.game_time);
    GET_FLOAT_CONFIG(config, "FreddyDeathDistance", game_loop_config.freddy_death_distance);
    GET_INT_CONFIG(config, "BunnyShowTime", game_loop_config.bunny_show_time);
    GET_INT_CONFIG(config, "BunnyShowRate", game_loop_config.bunny_show_rate);
    GET_INT_CONFIG(config, "BunnyReactTime", game_loop_config.bunny_react_time);
    GET_FLOAT_CONFIG(config, "SpeedIncreaseRate", game_loop_config.speed_increase_rate);
}

#undef SET_VEC3_CONFIG
#undef GET_VEC3_CONFIG
#undef GET_VEC3_CONFIG_DIR
#undef GET_FLOAT_CONFIG