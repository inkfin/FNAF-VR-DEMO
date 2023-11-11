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

const int kLinearActuatorBinding = 0;
const int kPointsBinding = 1;

layout (std430, binding = kLinearActuatorBinding) restrict buffer ACTUATORS 
{
	LinearActuator mLinearActuator[];
};

layout (std430, binding = kPointsBinding) restrict buffer PARTICLES_IN 
{
	XpbdParticle particles_in[];
};

const int SIM_MODE = 0;
const int PAUSE_MODE = 1;
const int DRAG_MODE = 2;

layout(location = 0) uniform int uMode = SIM_MODE; 
layout(location = 1) uniform int uNumElements=0;

void ProjLinearAct(int ix);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= uNumElements) return;
	ProjLinearAct(gid);
}

void ProjLinearAct(int ix)
{
	LinearActuator c = mLinearActuator[ix];
	if(c.mEnd[0]==-1 || c.mEnd[1]==-1) return;

	XpbdParticle pi = particles_in[c.mEnd[0]];
	XpbdParticle pj = particles_in[c.mEnd[1]];
	float wi = GetW(pi);
	float wj = GetW(pj);

	float l = distance(pi.xpos.xy, pj.xpos.xy);
	
	float C = 0.0;
	if(uMode == SIM_MODE)
	{
		c.mAngle += SimUbo.dt*c.mFreq;
		float la = (1.0-c.mAmplitude)*c.mL0 + c.mAmplitude*c.mL0*(0.5*cos(c.mAngle)+0.5);
		C = l-la;
	}
	if(uMode == PAUSE_MODE)
	{
		float la = (1.0-c.mAmplitude)*c.mL0 + c.mAmplitude*c.mL0*(0.5*cos(c.mAngle)+0.5);
		C = l-la;	
	}
	if(uMode == DRAG_MODE)
	{
		float cos_angle = ((l - (1.0-c.mAmplitude)*c.mL0)/(c.mAmplitude*c.mL0)-0.5)/0.5;
		cos_angle = clamp(cos_angle, -1.0, 1.0);
		c.mAngle = acos(cos_angle);
	}

	vec3 gradCi = (pi.xpos.xyz-pj.xpos.xyz)/(l+eps);
	vec3 gradCj = -gradCi;
	
	float lambda = -C/(wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + c.mAlpha/dt/dt + 1e-20);

	vec3 dxi = lambda*wi*gradCi;
	vec3 dxj = lambda*wj*gradCj;

	particles_in[c.mEnd[0]].xpos.xyz += dxi;
	particles_in[c.mEnd[1]].xpos.xyz += dxj;

	mLinearActuator[ix]=c; //write back since angle changed
}

