#version 450 

#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 6

layout (std430, binding = 0) readonly restrict buffer PARTICLES
{
	XpbdParticle particles[];
};

layout (std430, binding = 1) restrict buffer VERTICES
{
	vec4 vertices[];
};

const int mNumTraces = 10;

layout(location=10) uniform int L = 1000;
layout(location=100) uniform int write_index[mNumTraces]; //vertex index
layout(location=200) uniform int read_index[mNumTraces]; //particle read index

out VertexData
{
	flat int cull;
	flat float hw;
	float v;
} outData; 

void main(void)
{
	//particle index
	int pix = read_index[gl_InstanceID];
	if(pix<0) 
	{
		outData.cull = 1; 
		return;
	}
	XpbdParticle particle = particles[pix];

	//last index in buffer (the one to be written)
	int last_index = write_index[gl_InstanceID];
	int global_last_index = gl_InstanceID*L + last_index;
	//vertex index
	int vix = gl_InstanceID*L + (gl_VertexID+last_index+1)%L;
	
	if(vix==global_last_index)//write the vertex from particle data
	{
		XpbdParticle particle = particles[pix];
		vertices[global_last_index] = vec4(particle.xpos.xyz, 1.0);
	}
	
	vec4 v = vertices[vix];
	gl_Position = SceneUniforms.PV*SceneUniforms.M*v; //transform vertices and send result into pipeline
	
	outData.cull = 0;//false
	vec4 clip = SceneUniforms.P*SceneUniforms.M*vec4(GetRadius(particle), GetRadius(particle), 0.0, 0.0);
	outData.hw = 0.25*min(clip.x, clip.y);
	outData.v = float(gl_VertexID)/float(L);
}