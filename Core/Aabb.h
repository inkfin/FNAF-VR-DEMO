#pragma once

#include <glm/glm.hpp>

struct aabb2D
{
   glm::vec2 mMin = glm::vec2(+1e20f);
   glm::vec2 mMax = glm::vec2(-1e20f);

   aabb2D();
   aabb2D(glm::vec2 box_min, glm::vec2 box_max);
};

inline float area(const aabb2D& box)
{
   glm::vec2 diff = box.mMax-box.mMin;
   return diff.x*diff.y;
}

inline bool overlap(const aabb2D& a, const aabb2D& b)
{
   if(a.mMax.x < b.mMin.x || b.mMax.x < a.mMin.x) {return false;}
   if(a.mMax.y < b.mMin.y || b.mMax.y < a.mMin.y) {return false;}
   return true;
}

inline bool point_in(const glm::vec2& p, const aabb2D& b)
{
   if(p.x < b.mMin.x || p.x > b.mMax.x) {return false;}
   if(p.y < b.mMin.y || p.y > b.mMax.y) {return false;}
   return true;  
}

inline aabb2D intersection(const aabb2D& a, const aabb2D& b)
{
   aabb2D isect;
   isect.mMin = glm::max(a.mMin, b.mMin);
   isect.mMax = glm::min(a.mMax, b.mMax);
   return isect;
}

inline glm::vec2 center(const aabb2D& a)
{
   return 0.5f*(a.mMin + a.mMax);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




struct aabb3D
{
   glm::vec4 mMin = glm::vec4(+1e20f); //using vec4 for GPU alignment
   glm::vec4 mMax = glm::vec4(-1e20f);

   aabb3D();
   aabb3D(glm::vec3 box_min, glm::vec3 box_max);
};

inline float volume(const aabb3D& box)
{
   glm::vec4 diff = box.mMax-box.mMin;
   return diff.x*diff.y*diff.z;
}

inline bool overlap(const aabb3D& a, const aabb3D& b)
{
   if(a.mMax.x < b.mMin.x || b.mMax.x < a.mMin.x) {return false;}
   if(a.mMax.y < b.mMin.y || b.mMax.y < a.mMin.y) {return false;}
   if(a.mMax.z < b.mMin.z || b.mMax.z < a.mMin.z) {return false;}
   return true;
}

inline bool point_in(const glm::vec3& p, const aabb3D& b)
{
   if(p.x < b.mMin.x || p.x > b.mMax.x) {return false;}
   if(p.y < b.mMin.y || p.y > b.mMax.y) {return false;}
   if(p.z < b.mMin.z || p.z > b.mMax.z) {return false;}
   return true;  
}

inline aabb3D intersection(const aabb3D& a, const aabb3D& b)
{
   aabb3D isect;
   isect.mMin = glm::max(a.mMin, b.mMin);
   isect.mMax = glm::min(a.mMax, b.mMax);
   return isect;
}

inline glm::vec3 center(const aabb3D& a)
{
   return 0.5f*glm::vec3(a.mMin + a.mMax);
}
