#version 450  

#include "std_uniforms.h.glsl"
#line 5

layout(location = 1) uniform mat4 M;

out VertexData
{
   vec3 p;

} outData; 


	const vec4 cube[8] = vec4[]( vec4(-1.0, -1.0, -1.0, 1.0),
								 vec4(-1.0, +1.0, -1.0, 1.0),
								 vec4(+1.0, +1.0, -1.0, 1.0),
								 vec4(+1.0, -1.0, -1.0, 1.0),
								 vec4(-1.0, -1.0, +1.0, 1.0),
								 vec4(-1.0, +1.0, +1.0, 1.0),
								 vec4(+1.0, +1.0, +1.0, 1.0),
								 vec4(+1.0, -1.0, +1.0, 1.0));

	const int index[14] = int[](1, 0, 2, 3, 7, 0, 4, 1, 5, 2, 6, 7, 5, 4);

void main(void)
{
	int ix = index[gl_VertexID];
	vec4 v = cube[ix];

	gl_Position = SceneUniforms.PV*M*v; //get clip space coords out of quad array
	outData.p = v.xyz;
}