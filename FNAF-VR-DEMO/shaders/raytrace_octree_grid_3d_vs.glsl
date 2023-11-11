#version 430            

out VertexData
{
   vec2 tex_coord;
   vec2 clip;
} outData; 

const vec4 quad[4] = vec4[] (vec4(-1.0, 1.0, 0.0, 1.0), 
										vec4(-1.0, -1.0, 0.0, 1.0), 
										vec4( 1.0, 1.0, 0.0, 1.0), 
										vec4( 1.0, -1.0, 0.0, 1.0) );

void main(void)
{

    gl_Position = quad[ gl_VertexID ]; //get clip space coords out of quad array
	outData.clip = quad[ gl_VertexID ].xy;
    outData.tex_coord = 0.5*(quad[ gl_VertexID ].xy + vec2(1.0)); 

}