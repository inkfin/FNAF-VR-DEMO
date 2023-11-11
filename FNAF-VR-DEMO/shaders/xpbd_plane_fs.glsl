#version 450 
#include "std_uniforms.h.glsl"
#line 4

layout(binding = 0) uniform sampler2D diffuse_tex; 

layout(location = 0) uniform vec4 mPlane;
layout(location = 1) uniform int mGridSquares = 8;
layout(location = 3) uniform vec2 ray_uv = vec2(-1.0);

in VertexData
{
	vec4 pobj;	//object-space vertex position
	vec2 uv;
} inData; 

float sdCircle( vec2 p, float r )
{
  return length(p) - r;
}

vec2 opRep( in vec2 p, in vec2 c)
{
    vec2 q = mod(p+0.5*c,c)-0.5*c;
    return q;
}

out vec4 fragcolor;

float sdHandle(vec2 p, float r)
{
	float h_dist = sdCircle(opRep(p, vec2(0.5, 1.0)), r);
	h_dist = min(h_dist, sdCircle(opRep(p + vec2(0.0, 0.5), vec2(1.0, 1.0)), r));
	return h_dist;
}

void main(void)
{
	const vec4 h_color = vec4(0.1, 1.0, 0.2, 0.5);
	const float h_rad = 0.05;

	vec2 size = inData.uv*mGridSquares;
	vec2 grid = abs(fract(size - 0.5) - 0.5) / (2.0*fwidth(size));
	float line = min(grid.x, grid.y);
	float a = min(line, 1.0);
	float alpha = mix(1.75, 0.1, a);
	
	fragcolor = vec4(0.7, 0.2, 0.1, alpha);

	float h_dist = sdHandle(inData.uv, h_rad);
	float ray_dist = sdHandle(ray_uv, h_rad);
	float h_color_scale = 1.0;
	if(ray_dist < 0.0f && all(greaterThan(ray_uv, vec2(0.0))))
	{
		h_color_scale = 2.0;
	}
	fragcolor = mix(fragcolor, h_color_scale*h_color, step(0.0, -h_dist));		

	if(textureSize(diffuse_tex, 0).x > 0)
	{
		vec4 tex_color = texture(diffuse_tex, inData.uv);
		fragcolor = mix(fragcolor, tex_color, tex_color.a);
	}
	//fragcolor = vec4(inData.uv, 0.0, 1.0);
}