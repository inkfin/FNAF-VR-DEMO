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
	AreaConstraint mConstraint[];
};

out VertexData
{
	flat int constraint_id;
	flat uint flags;
	flat vec3 nw;
	vec3 pw;		//world-space vertex position
	vec3 peye;		//eye-space position
} outData; 

vec3 compute_normal(AreaConstraint acon)
{
	vec3 nw;
	vec3 p0 = particles[acon.mEnd[0]].xpos.xyz;
	vec3 p1 = particles[acon.mEnd[1]].xpos.xyz;
	vec3 p2 = particles[acon.mEnd[2]].xpos.xyz;

	return normalize(cross(p1-p0, p2-p0));
}

void main(void)
{
	outData.constraint_id = gl_VertexID/3;
	AreaConstraint acon = mConstraint[outData.constraint_id];
	if(IsDeleted(acon)) gl_Position.w = 0.0f;//cull particle
	outData.flags = acon.mFlags;
	outData.nw = compute_normal(acon);

	int end_ix = gl_VertexID%3;	
	int part_ix = int(acon.mEnd[end_ix]);

	XpbdParticle part = particles[part_ix];
	vec4 p = vec4(part.xpos.xyz, 1.0);
	vec4 pw = SceneUniforms.M*p;
	vec4 peye = SceneUniforms.V*pw;

	outData.pw = pw.xyz;
	outData.peye = peye.xyz;

	gl_Position = SceneUniforms.P*peye; //transform vertices and send result into pipeline
}
