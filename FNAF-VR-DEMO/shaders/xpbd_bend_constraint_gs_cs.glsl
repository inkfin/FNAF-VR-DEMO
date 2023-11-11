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
	BendingConstraint mConstraint[];
};

layout (std430, binding = kPointsBinding) restrict buffer PARTICLES_IN 
{
	XpbdParticle particles_in[];
};

layout(location = 0) uniform int uMode=0; 
layout(location = 1) uniform int uNumElements=0;

const int AREA_MODE = 0;
const int DOT_MODE = 1;
const int DOT2_MODE = 2;
const int DIST_MODE = 3;
const int CROSS_MODE = 4;
const int ROT_MODE = 5;

void ProjBendConstraint(int ix, int mode);
void ZeroLambda(int ix);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	if(gid >= mConstraint.length()) return;
	if(gid >= uNumElements) return;

	if(uMode==0) ZeroLambda(gid);
	else
	{
		ProjBendConstraint(gid, CROSS_MODE);
		ProjBendConstraint(gid, DOT_MODE);
	}
}

void ZeroLambda(int ix)
{
	mConstraint[ix].mLambda = 0.0;
}


void ProjBendConstraint(int ix, int mode)
{
	BendingConstraint c = mConstraint[ix];
	if(c.mEnd[0]==-1 || c.mEnd[1]==-1 || c.mEnd[2]==-1) return;

	XpbdParticle xi = particles_in[c.mEnd[0]];
	XpbdParticle xj = particles_in[c.mEnd[1]];
	XpbdParticle xk = particles_in[c.mEnd[2]];

	float wi = GetW(xi);
	float wj = GetW(xj);
	float wk = GetW(xk);
	if(wi==0.0 && wj==0.0 && wk==0.0) return;

	float ri = GetRadius(xi);
	float rj = GetRadius(xj);
	float rk = GetRadius(xk);

	vec3 pi = xi.xpos.xyz;
	vec3 pj = xj.xpos.xyz;	
	vec3 pk = xk.xpos.xyz;	

	vec3 pji = xj.xpos.xyz-xi.xpos.xyz;
	vec3 pki = xk.xpos.xyz-xi.xpos.xyz;
	vec3 pkj = xk.xpos.xyz-xj.xpos.xyz;

	float C;
	vec3 gradCi, gradCj, gradCk;

	if(mode==DOT_MODE)
	{
		float ct = cos(c.mAngle); 
		float Lkj = length(pkj);
		float Lji = length(pji);

		C = dot(pji, pkj) - ct*Lji*Lkj;
		//gradCi = -pkj + ct*pji*Lkj/Lji;
		//gradCj = pkj-pji - ct*pji*Lkj/Lji + ct*pkj*Lji/Lkj;
		//gradCk = pji - ct*pkj*Lji/Lkj;

		gradCi = -pkj;
		gradCj = pkj-pji;
		gradCk = pji;
	}

	if(mode==CROSS_MODE)
	{
		float st = abs(sin(c.mAngle));
		float Lkj = length(pkj);
		float Lji = length(pji);

		C = length(cross(pji, pkj)) - st*Lji*Lkj;

		float s1 = Lkj;
		float s2 = Lji+eps;
		float s4 = (pi.y-pj.y)*(pj.z-pk.z)-(pi.z-pj.z)*(pj.y-pk.y);
		float s5 = (pi.x-pj.x)*(pj.z-pk.z)-(pi.z-pj.z)*(pj.x-pk.x);
		float s6 = (pi.x-pj.x)*(pj.y-pk.y)-(pi.y-pj.y)*(pj.x-pk.x);
		float s3 = 2.0*sqrt(s6*s6 + s5*s5 + s4*s4)+eps;

		gradCi = vec3((2.0*s6*(pj.y-pk.y) + 2.0*s5*(pj.z-pk.z))/s3 -st*(pi.x-pj.x)*s1/s2,
					 -(2.0*s6*(pj.x-pk.x) - 2.0*s4*(pj.z-pk.z))/s3 -st*(pi.y-pj.y)*s1/s2,
					 -(2.0*s5*(pj.x-pk.x) + 2.0*s4*(pj.y-pk.y))/s3 -st*(pi.z-pj.z)*s1/s2);

		gradCj = vec3(-(2.0*s6*(pi.y-pk.y) + 2.0*s5*(pi.z-pk.z))/s3 +st*(pi.x-pj.x)*s1/s2 -st*(pj.x-pk.x*s2/s1),
					 (+2.0*s6*(pi.x-pk.x) - 2.0*s4*(pi.z-pk.z))/s3 +st*(pi.y-pj.y)*s1/s2 -st*(pj.y-pk.y*s2/s1),
					 (+2.0*s5*(pi.x-pk.x) + 2.0*s4*(pi.y-pk.y))/s3 +st*(pi.z-pj.z)*s1/s2 -st*(pj.z-pk.z*s2/s1));

		gradCk = vec3((2.0*s6*(pi.y-pj.y) + 2.0*s5*(pi.z-pj.z))/s3 +st*(pj.x-pk.x)*s2/s1,
					 -(2.0*s6*(pi.x-pj.x) - 2.0*s4*(pi.z-pj.z))/s3 +st*(pj.y-pk.y)*s2/s1,
					 -(2.0*s5*(pi.x-pj.x) + 2.0*s4*(pi.y-pj.y))/s3 +st*(pj.z-pk.z)*s2/s1);

	}

	//float lambda = -C/(wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + wk*dot(gradCk, gradCk) + c.mAlpha/dt/dt + 1e-20);
	
	float alpha = c.mAlpha/(SimUbo.dt*SimUbo.dt);
	float dLambda = (-C-alpha*c.mLambda)/(wi*dot(gradCi, gradCi) + wj*dot(gradCj, gradCj) + wk*dot(gradCk, gradCk) + alpha + 1e-20);

	particles_in[c.mEnd[0]].xpos.xyz += SimUbo.omega_bend*dLambda*wi*gradCi;
	particles_in[c.mEnd[1]].xpos.xyz += SimUbo.omega_bend*dLambda*wj*gradCj;
	particles_in[c.mEnd[2]].xpos.xyz += SimUbo.omega_bend*dLambda*wk*gradCk;

	c.mLambda += dLambda;
	mConstraint[ix] = c;

}

