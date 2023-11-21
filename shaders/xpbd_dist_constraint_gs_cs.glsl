#version 450

#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 7

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 1024
#endif
layout(local_size_x = LOCAL_SIZE) in;

const float eps = 1e-12;

const int kConstraintsBinding = 0;
const int kPointsBinding = 1;

layout (std430, binding = kConstraintsBinding) restrict buffer CONSTRAINTS 
{
	DistanceConstraint mConstraint[];
};

layout (std430, binding = kPointsBinding) restrict buffer PARTICLES_IN 
{
	XpbdParticle particles_in[];
};

layout(location = 0) uniform int uMode=0; 
layout(location = 1) uniform int uNumElements=0;

void ProjDistConstraint(int ix);
void ZeroLambda(int ix);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= uNumElements) return;
	if(gid >= mConstraint.length()) return;
	if(uMode==0) ZeroLambda(gid);
	else ProjDistConstraint(gid);
}

void ZeroLambda(int ix)
{
	mConstraint[ix].mLambda = 0.0;
}

void ProjDistConstraint(int ix)
{
	DistanceConstraint c = mConstraint[ix];
	if(c.mEnd[0]==-1 || c.mEnd[1]==-1) return;
	float l0 = c.mL0;

	XpbdParticle pi = particles_in[c.mEnd[0]];
	XpbdParticle pj = particles_in[c.mEnd[1]];
	float wi = GetW(pi);
	float wj = GetW(pj);
	if(wi==0.0 && wj==0.0) return;

	float l = distance(pi.xpos.xyz, pj.xpos.xyz);
	float C = l-l0;
	float slop = 0.0001; 
	if(C>slop) C-=slop;
	if(C<-slop) C+=slop;
	if(abs(C)<slop) C = 0.0; //slop
	vec3 gradCi = (pi.xpos.xyz-pj.xpos.xyz)/(l+eps);
	vec3 gradCj = -gradCi;
	
	//float lambda = -C/(wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + c.mAlpha/dt/dt + 1e-20);

	float alpha = c.mAlpha/(SimUbo.dt*SimUbo.dt);
	float dLambda = (-C-alpha*c.mLambda)/(wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + alpha + 1e-20);

	vec3 dxi = dLambda*wi*gradCi;
	vec3 dxj = dLambda*wj*gradCj;

	particles_in[c.mEnd[0]].xpos.xyz += SimUbo.omega_dist*dxi;
	particles_in[c.mEnd[1]].xpos.xyz += SimUbo.omega_dist*dxj;

	particles_in[c.mEnd[0]].vel.xyz = vec3(0.0);
	particles_in[c.mEnd[1]].vel.xyz = vec3(0.0);	

	c.mLambda += dLambda;
	mConstraint[ix] = c;
}

