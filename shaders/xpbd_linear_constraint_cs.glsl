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

const int kLinearActuatorBinding = 0;
const int kPointsBinding = 1;

layout (std430, binding = kLinearActuatorBinding) restrict buffer ACTUATORS 
{
	LinearConstraint mLinearConstraint[];
};

layout (std430, binding = kPointsBinding) restrict buffer PARTICLES_IN 
{
	XpbdParticle particles_in[];
};

layout(location = 0) uniform int uMode=0; 
layout(location = 1) uniform int uNumElements=0;

void ProjDistConstraint(int ix);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= uNumElements) return;
	ProjDistConstraint(gid);
}

vec3 closestPointToInfLine( vec3 p, vec3 a, vec3 b)
{
    vec3 pa = p-a, ba = b-a;
	float h = dot(pa,ba)/(dot(ba,ba)+1e-20);
    vec3 q = a + ba*h;
    return q;
}

float sdCapsule( vec3 p, vec3 a, vec3 b, float r )
{
  vec3 pa = p - a, ba = b - a;
  float h = clamp( dot(pa,ba)/(dot(ba,ba)+1e-20), 0.0, 1.0 );
  return length( pa - ba*h ) - r;
}

vec3 closestPointToCapsule( vec3 p, vec3 a, vec3 b, float r )
{
    vec3 pa = p-a, ba = b-a;
	float h = clamp( dot(pa,ba)/(dot(ba,ba)+1e-20), 0.0, 1.0 );
    vec3 q = a + ba*h;
    return q + r*normalize(p-q);
}

void ProjDistConstraint(int ix)
{
	LinearConstraint c = mLinearConstraint[ix];
	if(c.mEnd[0]==-1 || c.mEnd[1]==-1) return;

	XpbdParticle pi = particles_in[c.mEnd[0]];
	XpbdParticle pj = particles_in[c.mEnd[1]];
	XpbdParticle p = particles_in[GetParticle(c)];

	float ri = GetRadius(pi);
	float rj = GetRadius(pj);
	float r = GetRadius(p);
	
	float wi = GetW(pi);
	float wj = GetW(pj);
	float w = GetW(p);

	vec3 xi = pi.xpos.xyz;
	vec3 xj = pj.xpos.xyz;
	vec3 xji = xj-xi;
	vec3 u = normalize(xji);
	xi += (ri+r)*u;
	xj -= (rj+r)*u;

	float dist = sdCapsule(p.xpos.xyz, xi, xj, 0.0);
	float C = dist;

	vec3 closest = closestPointToCapsule(p.xpos.xyz, xi, xj, 0.0);
	vec3 gradC = (p.xpos.xyz-closest)/(dist+eps);
	
	
	

///////
	vec3 x = p.xpos.xyz - (r+ri)*u;
	float disti = sdCapsule(pi.xpos.xyz, x, x-10.0*u, 0.0);
	float Ci = disti;

	vec3 closesti = closestPointToCapsule(pi.xpos.xyz, x, x-10.0*u, 0.0);
	vec3 gradCi = (pi.xpos.xyz-closesti)/(disti+eps);
	
	
//////
	x = p.xpos.xyz + (r+rj)*u;
	float distj = sdCapsule(pj.xpos.xyz, x, x+10.0*u, 0.0);
	float Cj = distj;

	vec3 closestj = closestPointToCapsule(pj.xpos.xyz, x, x+10.0*u, 0.0);
	vec3 gradCj = (pj.xpos.xyz-closestj)/(distj+eps);
	
	
	float lambda = -C/(w*dot(gradC, gradC) + wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + c.mAlpha/dt/dt + 1e-20);
	float lambdai = -Ci/(w*dot(gradC, gradC) + wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + c.mAlpha/dt/dt + 1e-20);
	float lambdaj = -Cj/(w*dot(gradC, gradC) + wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + c.mAlpha/dt/dt + 1e-20);

	particles_in[GetParticle(c)].xpos.xyz += lambda*w*gradC;
	particles_in[c.mEnd[0]].xpos.xyz += 0.5*lambdai*wi*gradCi;
	particles_in[c.mEnd[1]].xpos.xyz += 0.5*lambdaj*wj*gradCj;

	//particles_in[c.mEnd[0]].xpos.xyz += 0.001*lambda*wi*gradCi;
	//particles_in[c.mEnd[1]].xpos.xyz += 0.001*lambda*wj*gradCj;
	
}

