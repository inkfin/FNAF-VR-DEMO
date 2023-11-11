#version 450

layout(binding = 0) uniform sampler2D diffuse_tex; 

#include "std_uniforms.h.glsl"

layout(location=1) uniform vec3 tint = vec3(1.0);

in VertexData
{
   vec2 tex_coord;
   vec3 pw;       //world-space vertex position
   vec3 nw;   //world-space normal vector
   vec3 peye; //eye-space position
} inData;   //block is named 'inData'

out vec4 fragcolor; //the output color for this fragment

void main(void)
{   
	//Compute per-fragment Phong lighting
	vec4 ktex = texture(diffuse_tex, inData.tex_coord);

	vec4 ambient_term = MaterialUniforms.ka*ktex*LightUniforms.La;

	const float eps = 1e-8; //small value to avoid division by 0
	float d = distance(LightUniforms.light_w.xyz, inData.pw.xyz);
	float atten = 1.0/(dot(LightUniforms.quad_atten.xyz, vec3(1.0, d, d*d))+eps);// quadratic attenutation

	vec3 nw = normalize(inData.nw);			//world-space unit normal vector
	vec3 lw = normalize(LightUniforms.light_w.xyz - inData.pw.xyz);	//world-space unit light vector
	vec4 diffuse_term = atten*MaterialUniforms.kd*ktex*LightUniforms.Ld*max(0.0, dot(nw, lw));

	vec3 vw = normalize(SceneUniforms.eye_w.xyz - inData.pw.xyz);	//world-space unit view vector
	vec3 rw = reflect(-lw, nw);	//world-space unit reflection vector

	vec4 specular_term = atten*MaterialUniforms.ks*LightUniforms.Ls*pow(max(0.0, dot(rw, vw)), MaterialUniforms.shininess);

	fragcolor = ambient_term + diffuse_term + specular_term;
	fragcolor.rgb *= tint;
	//fragcolor.rgb = nw;
	//fragcolor.rgb = lw;
	//fragcolor = diffuse_term;
}




















