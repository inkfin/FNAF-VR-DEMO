#include "iq.h"

namespace iq
{
   glm::vec3 yzx(const glm::vec3& v) { return glm::vec3(v.y, v.z, v.x); }
   glm::vec3 zxy(const glm::vec3& v) { return glm::vec3(v.z, v.x, v.y); }

   float planeIntersect(glm::vec3 ro, glm::vec3 rd, glm::vec4 p)
   {
      return -(glm::dot(ro, glm::vec3(p)) + p.w) / glm::dot(rd, glm::vec3(p));
   }

   // https://iquilezles.org/articles/boxfunctions
   glm::vec2 sBox(glm::vec3 ro, glm::vec3 rd, glm::mat4 txx, glm::vec3 rad)
   {
      glm::vec3 rdd = (txx * glm::vec4(rd, 0.0));
      glm::vec3 roo = (txx * glm::vec4(ro, 1.0));

      glm::vec3 m = glm::clamp(1.0f / rdd, -1e16f, 1e16f);
      glm::vec3 n = m * roo;
      glm::vec3 k = abs(m) * rad;

      glm::vec3 t1 = -n - k;
      glm::vec3 t2 = -n + k;

      float tN = glm::max(glm::max(t1.x, t1.y), t1.z);
      float tF = glm::min(glm::min(t2.x, t2.y), t2.z);
      if (tN > tF || tF < 0.0) return glm::vec2(-1.0);

      return glm::vec2(tN, tF);
   }

   //https://iquilezles.org/articles/distfunctions/
   float sdCircle(vec2 p, float r)
   {
      return length(p) - r;
   }

   vec2 opRep(vec2 p, vec2 c)
   {
      vec2 q = mod(p + 0.5f * c, c) - 0.5f * c;
      return q;
   }
}