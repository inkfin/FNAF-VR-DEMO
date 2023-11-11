#pragma once

#include "glm/glm.hpp"

namespace iq
{
   using namespace glm;

   vec3 yzx(const vec3& v);
   vec3 zxy(const vec3& v);
   float planeIntersect(vec3 ro, vec3 rd, vec4 p);
   vec2 sBox(glm::vec3 ro, glm::vec3 rd, mat4 txx, vec3 rad);
   float sdCircle(vec2 p, float r);
   vec2 opRep(vec2 p, vec2 c);
};
