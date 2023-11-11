#version 450 

#include "std_uniforms.h.glsl"
#include "xpbd_cs.h.glsl"
#line 6

layout (std430, binding = 0) readonly restrict buffer PARTICLES
{
	XpbdParticle particles[];
};

layout(location=5) uniform float uParticleSize = 1.0;

out VertexData
{
	vec3 pw;			//world-space vertex position
	vec3 peye;		//eye-space position
	vec3 color;
	float radius;
	float w;
	flat int vertex_id;
} outData; 

void main(void)
{
	XpbdParticle particle = particles[gl_VertexID];
	vec4 p = vec4(particle.xpos.xyz, 1.0);
	float r = GetRadius(particle);
	
	gl_Position = SceneUniforms.PV*SceneUniforms.M*p; 
	if(IsDeleted(particle)) gl_Position.w = 0.0f;//cull particle
	outData.peye = vec3(SceneUniforms.V*SceneUniforms.M*p);
	outData.pw = vec3(SceneUniforms.M*p);
	
	outData.vertex_id = gl_VertexID;

	float s = length(SceneUniforms.M[0].xyz);
	gl_PointSize = s*SceneUniforms.Viewport[3] * SceneUniforms.P[1][1] * r / gl_Position.w;
	outData.radius = s*r;
	outData.w = GetW(particle);
	outData.color = GetColor(particle).rgb;
	if(IsSelected(particle)) 
	{
		outData.color = mix(outData.color, vec3(2.0, 2.0, -1.0), 0.75);
		vec2 heat = unpackHalf2x16(particle.flags[1]);

		outData.color.r *= heat[0];
		outData.color.g *= heat[1];
	}
}