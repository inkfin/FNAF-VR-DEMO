#version 450
#include "aabb_cs.h.glsl"
#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 6

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 1024
#endif
layout(local_size_x = LOCAL_SIZE) in;


const int kGridUboBinding = 0;

const int kPointsInBinding = 0;
const int kPointsOutBinding = 1;
const int kCountBinding = 2;
const int kStartBinding = 3;
const int kContentBinding = 4;
const int kConstraintsBinding = 5;

const float eps = 1e-6;

layout (std430, binding = kPointsInBinding) restrict readonly buffer PARTICLES_IN 
{
	XpbdParticle particles_in[];
};

layout (std430, binding = kPointsOutBinding) restrict writeonly buffer PARTICLES_OUT 
{
	XpbdParticle particles_out[];
};

layout (std430, binding = kCountBinding) restrict readonly buffer GRID_COUNTER 
{
	int mCount[];
};

layout (std430, binding = kStartBinding) restrict readonly buffer GRID_START 
{
	int mStart[];
};

layout (std430, binding = kContentBinding) restrict readonly buffer CONTENT_LIST 
{
	int mContent[];
};

#include "grid_3d_cs.h.glsl"
#line 51

layout(location = 0) uniform int uMode=0; 
layout(location = 1) uniform int uNumElements=0;

const int MODE_INIT = 0;
const int MODE_UPDATE_X = 1;
const int MODE_CONSTRAINT = 2;
const int MODE_UPDATE_V = 3;
const int MODE_UPDATE_V_AND_X = 4;
const int MODE_CONSTRAINT_GRID = 5;
const int MODE_UNPAUSE = 6;

void UpdateX(int ix);
void SolveConstraints(int ix);
void UpdateV(int ix);

void UpdateVandX(int ix);
void SolveConstraintsGrid(int ix);

vec3 WallCollision(in XpbdParticle pi, vec4 plane);
void Unpause(int ix);


void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	
	switch(uMode)
	{
		case MODE_INIT:
			
		break;

		case MODE_UPDATE_X:
			UpdateX(gid);
		break;

		case MODE_CONSTRAINT:
			SolveConstraints(gid);
		break;

		case MODE_UPDATE_V:
			UpdateV(gid);
		break;

		case MODE_UPDATE_V_AND_X:
			UpdateVandX(gid);
		break;

		case MODE_CONSTRAINT_GRID:
			SolveConstraintsGrid(gid);
		break;

		case MODE_UNPAUSE:
			Unpause(gid);
		break;
	}
}

void Unpause(int ix)
{
	if(ix >= uNumElements) return;
	XpbdParticle pi = particles_in[ix];
	if(IsDeleted(pi)) return;
	pi.xprev.xyz = pi.xpos.xyz;
	pi.vel.xyz = vec3(0.0);
	particles_out[ix] = pi;
}

void UpdateX(int ix)
{
	if(ix >= uNumElements) return;
	XpbdParticle pi = particles_in[ix];
	if(IsDeleted(pi)) return;

	float wi = GetW(pi);
	//Apply gravity and other external forces
	if(wi > 0.0) pi.vel.xyz += SimUbo.g.xyz*SimUbo.dt;
	pi.xprev.xyz = pi.xpos.xyz;
	if(wi > 0.0) pi.xpos.xyz += pi.vel.xyz*SimUbo.dt;

	particles_out[ix] = pi;
}

void UpdateV(int ix)
{
	if(ix >= uNumElements) return;
	XpbdParticle pi = particles_in[ix];
	if(IsDeleted(pi)) return;
	//update with dx from constraints
	pi.xpos.xyz += pi.vel.xyz;
	pi.vel.xyz = SimUbo.c*(pi.xpos.xyz-pi.xprev.xyz)/SimUbo.dt;

	particles_out[ix] = pi;
}

void UpdateVandX(int ix)
{
	if(ix >= uNumElements) return;
	XpbdParticle pi = particles_in[ix];
	if(IsDeleted(pi)) return;
	float wi = GetW(pi);

	//update with dx from constraints
	pi.xpos.xyz += pi.vel.xyz;

	pi.vel.xyz = SimUbo.c*(pi.xpos.xyz-pi.xprev.xyz)/SimUbo.dt; //update V

	//Apply gravity and other external forces
	pi.xprev.xyz = pi.xpos.xyz;
	if(wi > 0.0) 
	{
		pi.vel.xyz += SimUbo.g.xyz*SimUbo.dt;
		pi.xpos.xyz += pi.vel.xyz*SimUbo.dt;
	}

	particles_out[ix] = pi;
}


void SolveConstraints(int ix)
{
	if(ix >= uNumElements) return;
	XpbdParticle pi = particles_in[ix];
	if(IsDeleted(pi)) return;
	vec3 dx = vec3(0.0);

	float ri = GetRadius(pi);
	float wi = GetW(pi);

	//wall collisions
	const vec4 wall[6] = vec4[](vec4(+1.0, 0.0, 0.0, -1.0),
								vec4(-1.0, 0.0, 0.0, -1.0),
								vec4(0.0, +1.0, 0.0, -1.0),
								vec4(0.0, -1.0, 0.0, -1.0),
								vec4(0.0, 0.0, +1.0, -1.0),
								vec4(0.0, 0.0, -1.0, -1.0));

	//Environment constraints
	for(int i=0; i<6; i++)
	{
		dx += WallCollision(pi, wall[i]);
	}
	
	//Collision constraints
	for(int jx=0; jx<uNumElements; jx++)
	{
		if(jx==ix) continue; //no self-interactions
		XpbdParticle pj = particles_in[jx];
		if(IsDeleted(pj)) return;

		float rj = GetRadius(pj);
		float wj = GetW(pj);
		vec3 nij = pi.xpos.xyz - pj.xpos.xyz;
		float d = length(nij)+eps;
		vec3 uij = nij/d;

		if(d < ri + rj)
		{
			//separate
			float h = 0.5f*(ri+rj-d);
			dx += wi*SimUbo.omega_collision*h*uij/(wi+wj);
		}
	}

	pi.xpos.xyz += dx;
	particles_out[ix] = pi;
}

void SolveConstraintsGrid(int ix)
{
	if(ix >= uNumElements) return;
	XpbdParticle pi = particles_in[ix];
	if(IsDeleted(pi)) return;

	vec3 dx = vec3(0.0);

	float ri = GetRadius(pi);
	float wi = GetW(pi);

	//wall collisions
	const vec4 wall[6] = vec4[](vec4(+1.0, 0.0, 0.0, -1.0),
								vec4(-1.0, 0.0, 0.0, -1.0),
								vec4(0.0, +1.0, 0.0, -1.0),
								vec4(0.0, -1.0, 0.0, -1.0),
								vec4(0.0, 0.0, +1.0, -1.0),
								vec4(0.0, 0.0, -1.0, -1.0));


	//Environment constraints
	for(int i=0; i<6; i++)
	{
		dx += WallCollision(pi, wall[i]);
	}
	
	float box_hw = 2.0*ri;
	aabb3D query_aabb = aabb3D(vec4(pi.xpos.xyz-vec3(box_hw), 0.0), vec4(pi.xpos.xyz+vec3(box_hw), 0.0));
	//These are the cells the query overlaps
	ivec3 cell_min = CellCoord(query_aabb.mMin.xyz);
	ivec3 cell_max = CellCoord(query_aabb.mMax.xyz);

	//Collision constraints
	for(int i=cell_min.x; i<=cell_max.x; i++)
	{
		for(int j=cell_min.y; j<=cell_max.y; j++)
		{
			for(int k=cell_min.z; k<=cell_max.z; k++)
			{
				int cell = Index(ivec3(i,j,k));
				int start = mStart[cell];
				int count = mCount[cell];

				for(int list_index = start; list_index<start+count; list_index++)
				{
					int jx = mContent[list_index];
					if(jx==ix) continue; //no self-interactions
					XpbdParticle pj = particles_in[jx];
					if(IsDeleted(pj)) continue;
					if((GetType(pi) == GetType(pj)) && (GetType(pi) != XpbdDefaultType)) continue;

					float rj = GetRadius(pj);
					float wj = GetW(pj);
					vec3 nij = pi.xpos.xyz - pj.xpos.xyz;
					float d = length(nij)+eps;
					vec3 uij = nij/d;

					if(d < ri + rj)
					{
						//separate
						float h = 0.5f*(ri+rj-d);
						dx += wi*SimUbo.omega_collision*h*uij/(wi+wj+eps);

						//friction
						float pen = clamp((ri+rj-d)/(ri+rj), 0.01, 1.0);
						vec3 dx_slide = (pi.vel.xyz-pj.vel.xyz)*SimUbo.dt;
						dx_slide = dx_slide-dot(dx_slide, uij)*uij;
						float mu = max(GetMu(pi), GetMu(pj));
						dx += -mu*pen*dx_slide;
					}
				}
			}
		}
	}
	
	pi.xpos.xyz += dx;
	pi.vel.xyz = vec3(0.0);

	particles_out[ix] = pi;
}


vec3 WallCollision(in XpbdParticle pi, vec4 plane)
{
	float ri = GetRadius(pi);
	float wi = GetW(pi);
	float d = dot(pi.xpos.xyz, plane.xyz)-plane.w;
	if(d >= ri || wi == 0.0) return vec3(0.0);

	//clamp position
	vec3 uij = plane.xyz;
	vec3 dx = SimUbo.omega_collision*(ri-d)*uij;

	//friction
	vec3 dx_slide = pi.vel.xyz*SimUbo.dt;
	dx_slide = dx_slide-dot(dx_slide, uij)*uij;
	float pen = clamp((ri-d)/ri, 0.1, 1.0);
	float mu = GetMu(pi);
	return dx-mu*pen*dx_slide;
	
}

