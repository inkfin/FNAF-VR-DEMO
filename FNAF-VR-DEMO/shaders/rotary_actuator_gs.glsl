#version 450

#include "std_uniforms.h.glsl"
#line 5

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

in VertexData
{
	float hw;
	flat int constraint_id;
	flat uint flags;
} inData[];

out GeomData
{
	vec3 pe;
	vec3 a;
	vec3 b;
	float hw;
	flat int constraint_id;
	flat uint flags;
} outData;

void main()
{
	float hw0 = inData[0].hw;
	float hw1 = inData[1].hw;

	vec4 p0 = gl_in[0].gl_Position; //eye-space
	vec4 p1 = gl_in[1].gl_Position;

	vec4 dir = normalize(p1-p0);
	vec4 n = normalize(vec4(-dir.y, dir.x, 0.0, 0.0));
	vec4 p;

	p = p0-hw0*dir-hw0*n;
	gl_Position = SceneUniforms.P*p;
	outData.pe = p.xyz;
	outData.a = p0.xyz;
	outData.b = p1.xyz;
	outData.hw = hw0;
	outData.flags = inData[0].flags;
	outData.constraint_id = inData[0].constraint_id;
	EmitVertex();

	p = p0-hw0*dir+hw0*n;
	gl_Position = SceneUniforms.P*p;
	outData.pe = p.xyz;
	outData.a = p0.xyz;
	outData.b = p1.xyz;
	outData.hw = hw0;
	outData.flags = inData[0].flags;
	outData.constraint_id = inData[0].constraint_id;
	EmitVertex();

	p = p1+hw1*dir-hw1*n;
	gl_Position = SceneUniforms.P*p;
	outData.pe = p.xyz;
	outData.a = p0.xyz;
	outData.b = p1.xyz;
	outData.hw = hw1;
	outData.flags = inData[1].flags;
	outData.constraint_id = inData[1].constraint_id;
	EmitVertex();

	p = p1+hw1*dir+hw1*n;
	gl_Position = SceneUniforms.P*p;
	outData.pe = p.xyz;
	outData.a = p0.xyz;
	outData.b = p1.xyz;
	outData.hw = hw1;
	outData.flags = inData[1].flags;
	outData.constraint_id = inData[1].constraint_id;
	EmitVertex();
	
	EndPrimitive();
}

