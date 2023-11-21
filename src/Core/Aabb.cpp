#include "Aabb.h"

aabb2D::aabb2D(glm::vec2 box_min, glm::vec2 box_max):mMin(box_min), mMax(box_max)
{

}

aabb2D::aabb2D():mMin(+1e20f), mMax(-1e20f)
{

}

aabb3D::aabb3D(glm::vec3 box_min, glm::vec3 box_max) :mMin(glm::vec4(box_min, 1.0f)), mMax(glm::vec4(box_max, 1.0))
{

}

aabb3D::aabb3D() :mMin(+1e20f), mMax(-1e20f)
{

}

