#pragma once

#include <unordered_map>
#include <string>

//These maps are const. Use map.at(key) or map.find(key) instead of map[key].

namespace GlEnumToString
{
   extern const std::unordered_map<unsigned int, std::string> type;
   extern const std::unordered_map<unsigned int, std::string> texture;
   extern const std::unordered_map<unsigned int, std::string> framebuffer;

   const std::string get_type(unsigned int key);
   const std::string get_tex(unsigned int key);
   const std::string get_fbo(unsigned int key);
   const std::string get(const std::unordered_map<unsigned int, std::string>& map, unsigned int key);
};
