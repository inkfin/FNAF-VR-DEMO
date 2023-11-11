#version 450

#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 5

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 1024
#endif
layout(local_size_x = LOCAL_SIZE) in;

const float eps = 1e-8;

const int kConstraintsBinding = 0;
const int kPointsBinding = 1;

layout (std430, binding = kConstraintsBinding) restrict buffer CONSTRAINTS 
{
	AreaConstraint mConstraint[];
};

layout (std430, binding = kPointsBinding) restrict buffer PARTICLES_IN 
{
	XpbdParticle particles_in[];
};

layout(location = 0) uniform int uMode=0; 
layout(location = 1) uniform int uNumElements=0;


void ProjAreaConstraint(int ix);
void ZeroLambda(int ix);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= mConstraint.length()) return;
	if(gid >= uNumElements) return;

	if(uMode==0) ZeroLambda(gid);
	else
	{
		ProjAreaConstraint(gid);
	}
}

void ZeroLambda(int ix)
{
	mConstraint[ix].mLambda = 0.0;
}

vec3 nearestLineSeg(vec3 p, vec3 a, vec3 b)
{
	const float eps = 1e-8;
	vec3 dir = b-a;
    float dSqr = dot(dir,dir);
    if(dSqr <= eps) return a;
    
    float t = clamp(dot(p - a, b - a) / dSqr, 0.0, 1.0);
    vec3 j = a + t * (b - a);
    
    return j;
}

vec3 nearestLine(vec3 p, vec3 a, vec3 b)
{
	const float eps = 1e-8;
	vec3 dir = b-a;
    float dSqr = dot(dir,dir);
    if(dSqr <= eps) return a;
    
    float t = dot(p - a, b - a) / dSqr;
    vec3 j = a + t * (b - a);
    
    return j;
}


void ProjAreaConstraint(int ix)
{
	AreaConstraint c = mConstraint[ix];
	if(c.mEnd[0]==-1 || c.mEnd[1]==-1 || c.mEnd[2]==-1) return;
	if(c.mArea<0.0) return;

	XpbdParticle xi = particles_in[c.mEnd[0]];
	XpbdParticle xj = particles_in[c.mEnd[1]];
	XpbdParticle xk = particles_in[c.mEnd[2]];

	float wi = GetW(xi);
	float wj = GetW(xj);
	float wk = GetW(xk);

	if(wi==0.0 && wj==0.0 && wk==0.0) return;

	vec3 pi = xi.xpos.xyz;
	vec3 pj = xj.xpos.xyz;	
	vec3 pk = xk.xpos.xyz;	

	float C;
	vec3 gradCi, gradCj, gradCk;

	vec3 cr = cross(pj-pi, pk-pi);
	float area = 0.5*length(cr);
	C = area - c.mArea;

	//move to midpoint
	//gradCi = pi-mix(pj, pk, 0.5);
	//gradCj = pj-mix(pi, pk, 0.5);
	//gradCk = pk-mix(pi, pj, 0.5);

	//move to nearest point on segment
	//gradCi = pi-nearestLineSeg(pi, pj, pk);
	//gradCj = pj-nearestLineSeg(pj, pi, pk);
	//gradCk = pk-nearestLineSeg(pk, pi, pj);

	//move to nearest point on inf line
	gradCi = pi-nearestLine(pi, pj, pk);
	gradCj = pj-nearestLine(pj, pi, pk);
	gradCk = pk-nearestLine(pk, pi, pj);

	//float lambda = -C/(wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + wk*dot(gradCk, gradCk) + c.mAlpha/dt/dt + 1e-20);
	
	float alpha = c.mAlpha/(SimUbo.dt*SimUbo.dt);
	float dLambda = (-C-alpha*c.mLambda)/(wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + wk*dot(gradCk, gradCk) + alpha + 1e-20);

	particles_in[c.mEnd[0]].xpos.xyz += SimUbo.omega_bend*dLambda*wi*gradCi;
	particles_in[c.mEnd[1]].xpos.xyz += SimUbo.omega_bend*dLambda*wj*gradCj;
	particles_in[c.mEnd[2]].xpos.xyz += SimUbo.omega_bend*dLambda*wk*gradCk;

	c.mLambda += dLambda;
	mConstraint[ix] = c;
}

