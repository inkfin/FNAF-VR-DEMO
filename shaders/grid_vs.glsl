#version 450
#include "std_uniforms.h.glsl"
#line 4

	const vec4 cube[8] = vec4[](	vec4(-1.0, -1.0, -1.0, 1.0),
									vec4(+1.0, -1.0, -1.0, 1.0),
									vec4(+1.0, +1.0, -1.0, 1.0),
									vec4(-1.0, +1.0, -1.0, 1.0),
									vec4(-1.0, -1.0, +1.0, 1.0),
									vec4(+1.0, -1.0, +1.0, 1.0),
									vec4(+1.0, +1.0, +1.0, 1.0),
									vec4(-1.0, +1.0, +1.0, 1.0));
	//face normals
	const vec3 n[6] = vec3[](	vec3(0.0, 0.0, +1.0),
								vec3(0.0, 0.0, -1.0),
								vec3(0.0, +1.0, 0.0),
								vec3(0.0, -1.0, 0.0),
								vec3(+1.0, 0.0, 0.0),
								vec3(-1.0, 0.0, 0.0));

	const vec2 uv[4] = vec2[](	vec2(0.0, 0.0),
								vec2(+1.0, 0.0),
								vec2(+1.0, +1.0),
								vec2(0.0, +1.0));

	const int vix[36] = int[](	0, 1, 2,	2, 3, 0,
								6, 5, 4,	4, 7, 6,
								5, 1, 0,	0, 4, 5,
								3, 2, 6,	6, 7, 3,
								0, 3, 7,	7, 4, 0,
								2, 1, 5,	5, 6, 2);

	const int nix[36] = int[](	0, 0, 0, 0, 0, 0,
								1, 1, 1, 1, 1, 1,
								2, 2, 2, 2, 2, 2,
								3, 3, 3, 3, 3, 3,
								4, 4, 4, 4, 4, 4,
								5, 5, 5, 5, 5, 5);

	const int uvix[36] = int[](0, 1, 2, 2, 3, 0,
								0, 1, 2, 2, 3, 0,
								0, 1, 2, 2, 3, 0,
								0, 1, 2, 2, 3, 0,
								0, 1, 2, 2, 3, 0,
								0, 1, 2, 2, 3, 0);

out VertexData
{
	vec3 pw;			//world-space vertex position
	vec3 peye;		//eye-space position
	vec3 nw;
	vec2 uv;
} outData; 

	
void main(void)
{
	vec4 v = cube[vix[gl_VertexID]];
	gl_Position = SceneUniforms.PV*SceneUniforms.M*v;

	outData.peye = vec3(SceneUniforms.V*SceneUniforms.M*v);
	outData.pw = v.xyz;
	outData.nw = n[nix[gl_VertexID]];
	outData.uv = uv[uvix[gl_VertexID]];
}