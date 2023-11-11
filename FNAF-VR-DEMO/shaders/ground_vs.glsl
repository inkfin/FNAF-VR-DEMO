#version 450  

#include "std_uniforms.h.glsl"
#line 5

out VertexData
{
   vec2 tex_coord;
   vec3 pw;			//world-space vertex position
   vec3 nw;			//world-space normal vector
   vec3 peye;		//eye-space position
} outData; 


//Ground is xz plane
const vec4 quad[4] = vec4[] (vec4(-1.0, 0.0, 1.0, 1.0), 
										vec4(-1.0, 0.0, -1.0, 1.0), 
										vec4( 1.0, 0.0, 1.0, 1.0), 
										vec4( 1.0, 0.0, -1.0, 1.0) );

const float s = 50.0;

void main(void)
{
	outData.pw = s*quad[ gl_VertexID ].xyz;
	gl_Position = SceneUniforms.PV*vec4(outData.pw, 1.0); //get clip space coords out of quad array
	outData.tex_coord = 0.5*(quad[ gl_VertexID ].xz + vec2(1.0));
	outData.nw = vec3(0.0, 0.0, 1.0);
}