struct aabb2D
{
   vec2 mMin;
   vec2 mMax;
};

bool overlap(aabb2D a, aabb2D b)
{
   if(a.mMax.x < b.mMin.x || b.mMax.x < a.mMin.x) {return false;}
   if(a.mMax.y < b.mMin.y || b.mMax.y < a.mMin.y) {return false;}
   return true;
}

bool point_in_aabb(vec2 pt, aabb2D box)
{
	if(all(greaterThan(pt, box.mMin)) && all(lessThan(pt, box.mMax))) return true;
	return false;
}

aabb2D intersection(aabb2D a, aabb2D b)
{
   aabb2D isect;
   isect.mMin = max(a.mMin, b.mMin);
   isect.mMax = min(a.mMax, b.mMax);
   return isect;
}

vec2 center(aabb2D a)
{
   return 0.5*(a.mMin + a.mMax);
}

float area(aabb2D box)
{
   vec2 diff = box.mMax-box.mMin;
   return diff.x*diff.y;
}

struct aabb3D
{
    vec4 mMin;
    vec4 mMax;
};

bool overlap(aabb3D a, aabb3D b)
{
   
    //if(any(lessThanEqual(a.mMax.xyz, b.mMin.xyz))) {return false;}
    //if(any(lessThanEqual(b.mMax.xyz, a.mMin.xyz))) {return false;}
    //return true;

    if(a.mMax.x < b.mMin.x || b.mMax.x < a.mMin.x) {return false;}
    if(a.mMax.y < b.mMin.y || b.mMax.y < a.mMin.y) {return false;}
    if(a.mMax.z < b.mMin.z || b.mMax.z < a.mMin.z) {return false;}
    return true;
}