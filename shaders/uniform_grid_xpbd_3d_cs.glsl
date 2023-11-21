#version 450
#include "aabb_cs.h.glsl"
#line 4

layout(local_size_x = 1024) in;

const int kGridUboBinding = 0;
const int kPointsInBinding = 0;
const int kPointsOutBinding = 1;
const int kCountBinding = 2;
const int kStartBinding = 3;
const int kContentBinding = 4;

//XpbdParticles
struct Particle
{
   vec4 pos;
   vec4 vel;    
   vec4 acc;  
   uvec4 flags;
};

layout (std430, binding = kPointsInBinding) readonly restrict buffer IN 
{
	Particle mParticles[];
};

//Modes
const int COMPUTE_COUNT = 0;
const int COMPUTE_START = 1;
const int INSERT_POINTS = 2;

layout(location=0) uniform int uMode = COMPUTE_COUNT;
layout(location=1) uniform int uNumElements = 0;

layout (std430, binding = kCountBinding) restrict buffer COUNTER 
{
	int mCount[];
};

layout (std430, binding = kStartBinding) restrict buffer START 
{
	int mStart[];
};

layout (std430, binding = kContentBinding) restrict buffer CONTENT
{
	int mContent[];
};

#include "grid_3d_cs.h.glsl"
#line 52

void ComputeCount(int gid);
void InsertPoint(int gid);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= uNumElements) return;
	//if(point_in_aabb(mParticles[gid].pos.xy, mExtents)==false) return;

	if(uMode==COMPUTE_COUNT)
	{
		ComputeCount(gid);	
	}
	else if(uMode==INSERT_POINTS)
	{
		InsertPoint(gid);
	}
}

void ComputeCount(int gid)
{
	//particle is a point and only belongs to a single cell
	ivec3 cell = CellCoord(mParticles[gid].pos.xyz);
	int ix = Index(cell);
	atomicAdd(mCount[ix], 1);
}

void InsertPoint(int gid)
{
	ivec3 cell = CellCoord(mParticles[gid].pos.xyz);
	int ix = Index(cell);
	int start = mStart[ix];
	int count = atomicAdd(mCount[ix], 1);
	mContent[start+count] = gid;
}
