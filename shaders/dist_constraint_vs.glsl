#version 450 

#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 6

layout (std430, binding = 0) readonly restrict buffer PARTICLES
{
	XpbdParticle particles[];
};

layout (std430, binding = 1) restrict buffer CONSTRAINTS 
{
	DistanceConstraint mConstraint[];
};


out VertexData
{
	float hw;
	flat int constraint_id;
	flat uint flags;
} outData; 

void main(void)
{
	outData.constraint_id = gl_VertexID/2;
	DistanceConstraint dcon = mConstraint[outData.constraint_id];
	if(IsDeleted(dcon)) gl_Position.w = 0.0f;//cull particle
	outData.flags = dcon.mFlags;

	int end_ix = gl_VertexID%2;	
	int part_ix = dcon.mEnd[end_ix];

	XpbdParticle part = particles[part_ix];
	float r = GetRadius(part);

	gl_Position = SceneUniforms.V*SceneUniforms.M*vec4(part.xpos.xyz, 1.0); //eye-space position
	
	float s = length(SceneUniforms.M[0].xyz);
	outData.hw = 0.5*s*r;
}