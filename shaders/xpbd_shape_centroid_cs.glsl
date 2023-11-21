#version 450
#extension GL_NV_shader_atomic_float : require

#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 7

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 1024
#endif
layout(local_size_x = LOCAL_SIZE) in;

const float eps = 1e-9;

const int kShapeParticleBinding = 0;
const int kParticleBinding = 1;
const int kConstraintsBinding = 2;

layout (std430, binding = kShapeParticleBinding) restrict readonly buffer SHAPE_PART_IN 
{
	ShapeParticle shape_part_in[];
};

layout (std430, binding = kParticleBinding) restrict buffer PARTICLES
{
	XpbdParticle particles[];
};

layout (std430, binding = kConstraintsBinding) restrict buffer CONSTRAINTS 
{
	ShapeMatchingConstraint mConstraint[];
};

layout(location = 0) uniform int uMode=0; 
layout(location = 1) uniform int uNumElements=0;

void CalcCentroid(int ix);
void CalcA(int ix);
void UpdateParticles(int ix);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= shape_part_in.length()) return;

	if(uMode==0) CalcCentroid(gid);
	if(uMode==1) CalcA(gid);
	if(uMode==2) UpdateParticles(gid);
}

void CalcCentroid(int ix)
{
	ShapeParticle shape_part = shape_part_in[ix];
	const int shape_id = shape_part.mShapeConstraintIndex;
	if(shape_id == -1) return;
	const int part_id = shape_part.mParticleIndex;
	if(part_id == -1) return;
	XpbdParticle part = particles[part_id];
	if(IsDeleted(part)) return;

	ShapeMatchingConstraint con = mConstraint[shape_id];
	float w = 1.0/float(con.mN[0]);

	//accumulate centroid
	for(int i=0; i<3; i++)
	{
		atomicAdd(mConstraint[shape_id].mCen[i], w*part.xpos[i]);
	}
}

//From "Unified Particle Physics for Real-Time Applications"
// https://dl.acm.org/doi/pdf/10.1145/2601097.2601152
void CalcA(int ix)
{
	ShapeParticle shape_part = shape_part_in[ix];
	const int shape_id = shape_part.mShapeConstraintIndex;
	if(shape_id == -1) return;
	const int part_id = shape_part.mParticleIndex;
	if(part_id == -1) return;
	XpbdParticle part = particles[part_id];
	if(IsDeleted(part)) return;

	vec3 xi = part.xpos.xyz;
	vec3 ri = shape_part.mRestPos.xyz;

	ShapeMatchingConstraint con = mConstraint[shape_id];
	vec3 c = con.mCen.xyz;

	mat3 Ai = outerProduct(xi-c, ri);

	//accumulate A
	for(int i=0; i<3; i++)
	for(int j=0; j<3; j++)
	{
		atomicAdd(mConstraint[shape_id].mA[i][j], Ai[i][j]);	
	}
}

void UpdateParticles(int ix)
{
	ShapeParticle shape_part = shape_part_in[ix];
	const int shape_id = shape_part.mShapeConstraintIndex;
	if(shape_id == -1) return;
	const int part_id = shape_part.mParticleIndex;
	if(part_id == -1) return;
	XpbdParticle part = particles[part_id];
	if(IsDeleted(part)) return;
	if(GetW(part)==0.0) return;

	ShapeMatchingConstraint con = mConstraint[shape_id];
	vec3 c = con.mCen.xyz;
	mat3 R = mat3(con.mR);
	vec3 ri = shape_part.mRestPos.xyz;

	vec3 shape_pos = R*ri + c;
	particles[part_id].xpos.xyz = mix(shape_pos, part.xpos.xyz, 0.75);

	/*
	vec3 delta = shape_pos-part.xpos.xyz;
	for(int i=0; i<3; i++)
	{
		atomicAdd(particles[part_id].xpos[i], delta[i]);
	}
	*/

	//debug
	//particles_in[ix].xpos.xyz = c;
	//particles_in[ix].xpos.xyz = ri;
	//particles_in[ix].xpos.xyz = c+ri;
	//particles_in[ix].xpos.xyz = R*ri;
	//particles_in[ix].xpos.xyz = R*ri+c;
}