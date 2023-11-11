#include "AttriblessRendering.h"
#include <GL/glew.h>

/*
Be sure to bind an attribless vao before calling draw* functions.
*/

void bind_attribless_vao()
{
	static GLuint attribless_vao = -1;
	static GLuint attribless_vbo = -1;
	if(attribless_vao == -1)
	{
		glGenVertexArrays(1, &attribless_vao);
		glGenBuffers(1, &attribless_vbo);
		glBindVertexArray(attribless_vao);
		glBindBuffer(GL_ARRAY_BUFFER, attribless_vbo);
	}
	glBindVertexArray(attribless_vao);
}


/*
draw_attribless_cube

In vertex shader declare:

	const vec4 cube[8] = vec4[]( vec4(-1.0, -1.0, -1.0, 1.0),
								 vec4(-1.0, +1.0, -1.0, 1.0),
								 vec4(+1.0, +1.0, -1.0, 1.0),
								 vec4(+1.0, -1.0, -1.0, 1.0),
								 vec4(-1.0, -1.0, +1.0, 1.0),
								 vec4(-1.0, +1.0, +1.0, 1.0),
								 vec4(+1.0, +1.0, +1.0, 1.0),
								 vec4(+1.0, -1.0, +1.0, 1.0));

	const int index[14] = int[](1, 0, 2, 3, 7, 0, 4, 1, 5, 2, 6, 7, 5, 4);

In vertex shader main() use:

	int ix = index[gl_VertexID];
	vec4 v = cube[ix];
	gl_Position = PV*v;

*/
void draw_attribless_cube()
{
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
}


/*
draw_attribless_wireframe_cube

In vertex shader declare:

	const vec4 cube[8] = vec4[]( vec4(-1.0, -1.0, -1.0, 1.0),
								 vec4(-1.0, +1.0, -1.0, 1.0),
								 vec4(+1.0, +1.0, -1.0, 1.0),
								 vec4(+1.0, -1.0, -1.0, 1.0),
								 vec4(-1.0, -1.0, +1.0, 1.0),
								 vec4(-1.0, +1.0, +1.0, 1.0),
								 vec4(+1.0, +1.0, +1.0, 1.0),
								 vec4(+1.0, -1.0, +1.0, 1.0));

	const int index[24] = int[](0,1,  1,2,  2,3,  3,0,
								0,4,  1,5,  2,6,  3,7,
								4,5,  5,6,  6,7,  7,4);

In vertex shader main() use:

	int ix = index[gl_VertexID];
	vec4 v = cube[ix];
	gl_Position = PV*v;

*/
void draw_attribless_wireframe_cube()
{
	glDrawArrays(GL_LINES, 0, 24);
}


/*
draw_attribless_lit_cube

In vertex shader declare:

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
	gl_Position = SceneUniforms.PV*v;

	outData.pw = v.xyz;
	outData.nw = n[nix[gl_VertexID]];
	outData.uv = uv[uvix[gl_VertexID]];
}

*/
void draw_attribless_lit_cube()
{
   glDrawArrays(GL_TRIANGLES, 0, 36);
}

/*
draw_attribless_quad

In vertex shader declare:
const vec4 quad[4] = vec4[] (vec4(-1.0, 1.0, 0.0, 1.0), 
										vec4(-1.0, -1.0, 0.0, 1.0), 
										vec4( 1.0, 1.0, 0.0, 1.0), 
										vec4( 1.0, -1.0, 0.0, 1.0) );


In vertex shader main() use:

	gl_Position = quad[ gl_VertexID ]; //get clip space coords out of quad array

*/

void draw_attribless_quad()
{
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // draw quad
}


/*
draw_attribless_triangle_grid

layout (location=1000) uniform ivec2 nxy = ivec2(10, 10);
out vec2 tex_coord;

//The rectangle that gets covered by an nxy.x x nxy.y mesh of vertices
const vec4 rect[4] = vec4[](vec4(-1.0, -1.0, 0.0, 1.0), vec4(+1.0, -1.0, 0.0, 1.0),
							vec4(-1.0, +1.0, 0.0, 1.0), vec4( +1.0, +1.0, 0.0, 1.0));

const ivec2 offset[6] = ivec2[](ivec2(0,0), ivec2(1,0), ivec2(0, 1), ivec2(1, 0), ivec2(0, 1), ivec2(1,1));

//This is just generating a grid in attributeless fashion
void grid_vertex(out vec4 pos, out vec2 uv)
{
	ivec2 qxy = nxy - ivec2(1); //number of rows and columns of quads
	int q = gl_VertexID/6;	//1D quad index (two triangles)
	int v = gl_VertexID%6;	//vertex index within the quad
	ivec2 ij = ivec2(q%qxy.x, q/qxy.x); //2D quad index of current vertex
	ij += offset[v]; //2D grid index of each point
	uv = ij/vec2(qxy);
	pos = mix(mix(rect[0], rect[1], uv.s), mix(rect[2], rect[3], uv.s), uv.t);
}

void main(void)
{
	vec4 pos;
	vec2 uv;
	grid_vertex(pos, uv);
	float height = textureLod(diffuse_tex, uv, 0.0).r;
	pos.z = 4.0*height;
	gl_Position = PVM*pos;
	tex_coord = uv;
}

*/

void draw_attribless_triangle_grid(int nx, int ny)
{
	const int GRID_UNIFORM_LOCATION = 1000;
	if(nx <= 1 || ny <= 1) return;
	//nx,ny: number of rows and columns of vertices
	int n = (nx-1)*(ny-1)*6;
	glUniform2i(GRID_UNIFORM_LOCATION, nx, ny);
	glDrawArrays(GL_TRIANGLES, 0, n);
}


//Get positions out of Ssbo
void draw_attribless_particles(int n)
{
	glDrawArrays(GL_POINTS, 0, n);
}