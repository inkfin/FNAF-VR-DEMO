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

layout (std430, binding = kPlanarConstraintBinding) restrict buffer CONSTRAINTS 
{
	PlanarConstraint mPlanarConstraint[];
};

layout (std430, binding = kPointsBinding) restrict buffer PARTICLES_IN 
{
	XpbdParticle particles_in[];
};

layout(location = 0) uniform int uMode=0; 
layout(location = 1) uniform int uNumElements=0;

void ProjPlanarConstraint(int ix);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= uNumElements) return;
	ProjPlanarConstraint(gid);
}


void ProjPlanarConstraint(int ix)
{
	PlanarConstraint c = mPlanarConstraint[ix];
	if(c.mEnd[0]==-1 || c.mEnd[1]==-1 || c.mEnd[2]==-1) return;

	XpbdParticle pi = particles_in[c.mEnd[0]];
	XpbdParticle pj = particles_in[c.mEnd[1]];
	XpbdParticle pk = particles_in[c.mEnd[2]];

	float wk = GetW(pk);

	vec3 u = normalize(pj.xpos.xyz-pi.xpos.xyz);

	vec3 x = pk.xpos.xyz-pj.xpos.xyz;
	vec3 target = pk.xpos.xyz-dot(u,x)*u;//project onto plane (pj,u)

	float dist = distance(pk.xpos.xyz, target);
	float C = dist;

	vec3 gradCk = (pk.xpos.xyz-target)/(dist+eps);
	float lambda = -C/(wk*dot(gradCk, gradCk) + c.mAlpha/dt/dt + 1e-20);

	vec3 dxk = lambda*wk*gradCk;
	particles_in[c.mEnd[2]].xpos.xyz += dxk;
}

