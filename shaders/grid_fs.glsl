#version 450 
#include "std_uniforms.h.glsl"
#line 4

layout(binding = 2) uniform sampler2DShadow shadow_map;
layout(location = 10) uniform mat4 Shadow; //shadow matrix

layout(location = 0) uniform float polygon_offset = 0.0;
layout(location = 1) uniform int mGridSquares = 8;

in VertexData
{
	vec3 pw;			//world-space vertex position
	vec3 peye;		//eye-space position
	vec3 nw;
	vec2 uv;
} inData; 

out vec4 fragcolor;

float diskOcclusion(in vec3 pos, in vec3 nor, in vec3 c_disk, in vec3 n_disk, in float r_disk);
float box_occlusion(in vec3 pos, in vec3 nor);

void main(void)
{
	vec4 shadow_coord = Shadow*vec4(inData.peye, 1.0);
	shadow_coord.z -= polygon_offset; //polygon offset
	float lit = 1.0;
	if(textureSize(shadow_map, 0).x > 0)
	{
		lit = textureProj(shadow_map, shadow_coord);
	}

	vec3 nw = normalize(inData.nw);			//world-space unit normal vector
	vec3 lw = normalize(LightUniforms.light_w.xyz - inData.pw.xyz);	//world-space unit light vector

	vec4 kd = MaterialUniforms.kd;

	vec2 size = inData.uv*mGridSquares;
	vec2 grid = abs(fract(size - 0.5) - 0.5) / (2.0*fwidth(size));
	float line = min(grid.x, grid.y);
	float color = min(line, 1.0);
	kd *= (0.5*color+0.5);

	vec4 ambient_term = kd*LightUniforms.La;
	vec4 diffuse_term = kd*LightUniforms.Ld*max(0.0, dot(nw, lw));

	vec3 vw = normalize(SceneUniforms.eye_w.xyz - inData.pw.xyz);	//world-space unit view vector
	vec3 rw = reflect(-lw, nw);	//world-space unit reflection vector

	vec4 specular_term = MaterialUniforms.ks*LightUniforms.Ls*pow(max(0.0, dot(rw, vw)), MaterialUniforms.shininess);

	float ao = box_occlusion(inData.pw.xyz, nw);
	vec4 phong = ao*ambient_term + (0.75*lit+0.25)*(diffuse_term + specular_term);

	fragcolor = phong;

	//fragcolor = vec4(lit); //debug shadow	
	//fragcolor = vec4(inData.uv, 0.0, 1.0); //debug uv	
	//fragcolor = vec4(abs(inData.nw), 1.0); //debug normal
}

float diskOcclusion( in vec3 pos, in vec3 nor, in vec3 c_disk, in vec3 n_disk, in float r_disk )
{
    vec3 v = c_disk-pos;
    float d = length(v);
    float num = dot(n_disk, v)*max(1.0, 4.0*dot(nor, v)/d);
    float den = sqrt(r_disk*r_disk + d*d);
    return 1.0-pow(abs(num/den), 0.25);
}

float box_occlusion(in vec3 pos, in vec3 nor)
{
	float occ = 1.0;
	const float r = 1.5;
	occ *= 1.0-diskOcclusion(pos, nor, nor.yzx, -nor.yzx, r);
	occ *= 1.0-diskOcclusion(pos, nor, -nor.yzx, nor.yzx, r);
	occ *= 1.0-diskOcclusion(pos, nor, nor.zxy, -nor.zxy, r);
	occ *= 1.0-diskOcclusion(pos, nor, -nor.zxy, nor.zxy, r);

	return occ;
}