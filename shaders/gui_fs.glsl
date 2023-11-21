#version 450
#include "picking.h.glsl"
#line 4

out vec4 fragcolor;   

layout(binding=0) uniform sampler2D tex;
layout(binding=1) uniform sampler2D back_tex;


in VertexData
{
	vec4 pw;
	vec2 tex_coord;
} inData; 

void main(void)
{   
	if(gl_FrontFacing) fragcolor = texture(tex, inData.tex_coord);
	else fragcolor = texture(back_tex, vec2(1.0-inData.tex_coord.x, inData.tex_coord.y));

	/*
	const float eps = 0.001;
	vec3 p_left, p_right;
    float id_left = GetLeftPick(p_left);
    float id_right = GetRightPick(p_right);
	const float r_pointer = 0.01;
    float d = distance(p_left.xyz, inData.pw.xyz) - r_pointer;
    fragcolor = mix(fragcolor, vec4(0.0), smoothstep(eps, 0.0, d));
	d = distance(p_right.xyz, inData.pw.xyz) - r_pointer;
    fragcolor = mix(fragcolor, vec4(0.0), smoothstep(eps, 0.0, d));
	*/

	PickPassOutput(inData.pw.xyz, 0);

	if(fragcolor.a < 0.5) discard;
}




















