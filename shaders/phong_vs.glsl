#version 450 

#include "std_uniforms.h.glsl"
#line 5

layout(location=0) uniform mat4 M;

layout(location = 0) in vec3 pos_attrib; 
layout(location = 1) in vec2 tex_coord_attrib;
layout(location = 2) in vec3 normal_attrib; 

out VertexData
{
   vec2 tex_coord;
   vec3 pw;			//world-space vertex position
   vec3 nw;			//world-space normal vector
   vec3 peye;		//eye-space position
} outData; 

void main(void)
{
	//Test mouse vars from SceneUniforms
	//vec3 scale = vec3(1.0);
	//scale.xy = 0.1*abs(SceneUniforms.MouseClickAndDrag.xy-SceneUniforms.MouseClickAndDrag.zw)+vec2(1.0);

	gl_Position = SceneUniforms.PV*M*vec4(pos_attrib, 1.0); //transform vertices and send result into pipeline
	outData.tex_coord = tex_coord_attrib;
	outData.pw = vec3(M*vec4(pos_attrib, 1.0));		//world-space vertex position
	outData.nw = vec3(M*vec4(normal_attrib, 0.0));	//world-space normal vector
	outData.peye = vec3(SceneUniforms.V*vec4(outData.pw, 1.0));
}