#version 450

#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 6

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 1024
#endif
layout(local_size_x = LOCAL_SIZE) in;

uniform float dt = 0.0005; //timestep
uniform float omega = 1.0;
const float eps = 1e-6;

const int kPlanarConstraintBinding = 0;
const int kPointsBinding = 1;
const int kPlanesBinding = 2;

layout (std430, binding = kPlanarConstraintBinding) restrict buffer CONSTRAINTS 
{
	StaticPlanarConstraint mStaticPlanarConstraint[];
};

layout (std430, binding = kPointsBinding) restrict buffer PARTICLES_IN 
{
	XpbdParticle particles_in[];
};

layout (std430, binding = kPlanesBinding) restrict readonly buffer PLANES_IN 
{
	ConstraintPlane planes_in[];
};

layout(location = 0) uniform int uMode=0; 
layout(location = 1) uniform int uNumElements=0;

void ProjPlanarConstraint(int ix);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= mStaticPlanarConstraint.length()) return;
	ProjPlanarConstraint(gid);
}


void ProjPlanarConstraint(int ix)
{
	StaticPlanarConstraint c = mStaticPlanarConstraint[ix];
	if(IsDeleted(c)) return;

	ConstraintPlane plane = planes_in[GetPlane(c)];
	//plane.xyz = normalize(plane.xyz);
	XpbdParticle p = particles_in[GetParticle(c)];

	float C = dot(plane.mEqn, vec4(p.xpos.xyz, 1.0));

	vec3 gradC = plane.mEqn.xyz;
	float lambda = -C/(dot(gradC, gradC) + c.mAlpha/dt/dt + 1e-20);

	vec3 dx = lambda*gradC;
	p.xpos.xyz += dx;

	const int mGridSquares = 8;

	//apply snap force
	if(plane.mSnapEnabled != 0)
	{
		//p.xpos.xyz = round(float(plane.mGridSquares) * p.xpos.xyz) / float(plane.mGridSquares);
		
		vec3 target = round(float(plane.mGridSquares) * p.xpos.xyz) / float(plane.mGridSquares);
		C = distance(p.xpos.xyz, target);
		gradC = (p.xpos.xyz - target);
		gradC -= dot(gradC, plane.mEqn.xyz)*plane.mEqn.xyz;
		gradC = gradC/(length(gradC)+1e-20);
		lambda = -C/(dot(gradC, gradC) + c.mAlpha/dt/dt + 1e-20);
		dx = 0.9*lambda*gradC;
		p.xpos.xyz += dx;
		
	}

	particles_in[GetParticle(c)].xpos.xyz = p.xpos.xyz;
}

