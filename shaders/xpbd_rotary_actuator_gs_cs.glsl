#version 450

#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 5

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 1024
#endif
layout(local_size_x = LOCAL_SIZE) in;

uniform float dt = 0.0005; //timestep
uniform float omega = 1.0;
const float eps = 1e-6;

const int kRotaryActuatorBinding = 0;
const int kPointsBinding = 1;

layout (std430, binding = kRotaryActuatorBinding) restrict buffer ACTUATORS 
{
	RotaryActuator mRotaryActuator[];
};

layout (std430, binding = kPointsBinding) restrict buffer PARTICLES_IN 
{
	XpbdParticle particles_in[];
};

const int SIM_MODE = 0;
const int PAUSE_MODE = 1;
const int DRAG_MODE = 2;

layout(location = 0) uniform int uMode=SIM_MODE; 
layout(location = 1) uniform int uNumElements=0;

void ProjRotaryActuator(int ix);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= uNumElements) return;
	ProjRotaryActuator(gid);
}

//Rodrigues' formula
mat3 rotate(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.y * axis.x - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.z * axis.y - axis.x * s,
                oc * axis.x * axis.z - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c         );
}

void ProjRotaryActuator(int ix)
{
	RotaryActuator c = mRotaryActuator[ix];
	if(c.mEnd[0]==-1 || c.mEnd[1]==-1 || c.mEnd[2]==-1) return;

	XpbdParticle pi = particles_in[c.mEnd[0]]; //axis0
	XpbdParticle pj = particles_in[c.mEnd[1]]; //axis1
	XpbdParticle pk = particles_in[c.mEnd[2]]; //rotating particle

	float wi = GetW(pi);
	float wj = GetW(pj);
	float wk = GetW(pk);

	vec3 u = normalize(pj.xpos.xyz-pi.xpos.xyz);
	vec3 omega = c.mOmega*u;
	vec3 r = pk.xpos.xyz-pj.xpos.xyz;
	//r = r-dot(u,r)*u;

	vec3 r_target = rotate(u, c.mOmega*SimUbo.dt)*r;

	vec3 v_perp = cross(omega, r);

	vec3 target;
	if(uMode==SIM_MODE)
	{
		//target = pk.xpos.xyz + v_perp*SimUbo.dt; //update with perp comp of velocity
		target = pj.xpos.xyz + r_target;
	}
	if(uMode==PAUSE_MODE)
	{
		target = pk.xpos.xyz;
		//return;
	}
	if(uMode==DRAG_MODE)
	{
		target = pk.xpos.xyz;
		//return;
	}

	vec3 x = target-pj.xpos.xyz;
	target = target-dot(u,x)*u; //project onto plane (pj,u)

	float dist = distance(pk.xpos.xyz, target);
	float C = dist;

	vec3 gradCi = (target-pk.xpos.xyz)/(dist+eps);
	vec3 gradCj = (target-pk.xpos.xyz)/(dist+eps);
	vec3 gradCk = (pk.xpos.xyz-target)/(dist+eps);
	wi = 0.0f;
	float lambda = -C/(wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + wk*dot(gradCk, gradCk) + c.mAlpha/dt/dt + 1e-20);

	vec3 dxi = lambda*wi*gradCi;
	vec3 dxj = lambda*wj*gradCj;
	vec3 dxk = lambda*wk*gradCk;

	//particles_in[c.mEnd[0]].xpos.xyz += dxi;
	particles_in[c.mEnd[1]].xpos.xyz += dxj;
	particles_in[c.mEnd[2]].xpos.xyz += dxk;

	//particles_in[c.mEnd[1]].xpos.xyz += pk.xpos.xyz-target;
	//particles_in[c.mEnd[2]].xpos.xyz += target-pk.xpos.xyz;

	//mRotaryActuator[ix] = c;
}

