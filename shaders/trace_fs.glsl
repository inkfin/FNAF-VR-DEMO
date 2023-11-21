#version 450
#include "std_uniforms.h.glsl"
#include "picking_cs.h.glsl"
#line 5
 
layout(location = 1) uniform float time;
layout(location = 2) uniform int pass;

in GeomData
{
	vec2 uv;
} inData;   

layout(location = 0)out vec4 fragcolor; //the output color for this fragment    

void main(void)
{   
	float u = 1.0-2.0*inData.uv[0];
	float v = inData.uv[1];
	vec3 color = vec3(1.0, 0.24, 0.44)*smoothstep(0.75, 0.70, abs(u))*v;
	float alpha = v*smoothstep(1.0, 0.95, abs(u));
	fragcolor = vec4(color, alpha);
}




















