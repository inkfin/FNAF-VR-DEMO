#version 450
#include "std_uniforms.h.glsl"
#line 4

layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

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

void generate_quad(vec4 p0, vec4 p1, float hw0, float hw1);

void main()
{
	float hw0 = inData[0].hw;
	float hw1 = inData[1].hw;
	float hw2 = inData[2].hw;

	vec4 p0 = gl_in[0].gl_Position; //eye-space
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;

	//p0.z -= 0.01;
	//p1.z -= 0.01;
	//p2.z -= 0.01;

	generate_quad(p0, p1, hw0, hw1);
	generate_quad(p1, p2, hw1, hw2);
}

void generate_quad(vec4 p0, vec4 p1, float hw0, float hw1)
{
	vec4 dir = normalize(p1-p0);
	vec4 n = vec4(-dir.y, dir.x, 0.0, 0.0);
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