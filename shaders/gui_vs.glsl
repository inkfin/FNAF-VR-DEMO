#version 450        

layout(location = 0) uniform mat4 PVM;
layout(location = 1) uniform mat4 M;

out VertexData
{
	vec4 pw;
	vec2 tex_coord;
} outData; 

const vec4 quad[4] = vec4[] (vec4(-1.0, 1.0, 0.0, 1.0), 
							vec4(-1.0, -1.0, 0.0, 1.0), 
							vec4( 1.0, 1.0, 0.0, 1.0), 
							vec4( 1.0, -1.0, 0.0, 1.0) );

void main(void)
{
	gl_Position = PVM*quad[gl_VertexID]; //get clip space coords out of quad array
	outData.tex_coord = 0.5*(quad[gl_VertexID].xy + vec2(1.0));
	outData.pw = M*quad[gl_VertexID];
}