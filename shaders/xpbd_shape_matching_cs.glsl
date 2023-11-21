#version 450

#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 7

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 1024
#endif
layout(local_size_x = LOCAL_SIZE) in;

const float eps = 1e-9;

const int kConstraintsBinding = 0;
const int kParticleBinding = 1;


layout (std430, binding = kConstraintsBinding) restrict buffer CONSTRAINTS 
{
	ShapeMatchingConstraint mConstraint[];
};

layout(location = 0) uniform int uMode=0; 
layout(location = 1) uniform int uNumElements=0;

void ZeroA(int ix);
void UpdateR(int ix);
mat3 rotate(vec3 axis, float angle);

void main()
{
	int gid = int(gl_GlobalInvocationID.x);
	//if(gid >= uNumElements) return;
	if(gid >= mConstraint.length()) return;
	if(uMode==0) ZeroA(gid);
	if(uMode==1) UpdateR(gid);
}

void ZeroA(int ix)
{
	mConstraint[ix].mA = mat4(0.0);
	mConstraint[ix].mCen = vec4(0.0);
}


//From A Robust Method to Extract the Rotational Part of Deformations
// https://matthias-research.github.io/pages/publications/stablePolarDecomp.pdf
void UpdateR(int ix)
{
	ShapeMatchingConstraint con = mConstraint[ix];
	if(con.mN[0] <= 0) return;

	mat3 R = mat3(con.mR);
	mat3 A = mat3(con.mA);

	const int max_iter = 3;
	for(int i=0; i<max_iter; i++)
	{
		vec3 omega = cross(R[0],A[0]) + cross(R[1],A[1]) + cross(R[2],A[2]);
		omega = omega/abs(dot(R[0],A[0]) + dot(R[1],A[1]) + dot(R[2],A[2]) + eps);
		float w = length(omega);
		if(w<eps) break;
		R = rotate(omega, -w)*R;
	}

	mConstraint[ix].mR = mat4(R);
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