#version 430  
layout(location = 0) uniform mat4 PV;
layout(location = 1) uniform mat4 M;
layout(location = 2) uniform float time;
layout(location = 3) uniform int num_bones = 0;
layout(location = 4) uniform int Mode = 0;
layout(location = 5) uniform int debug_id = 0;
const int MAX_BONES = 100;
layout(location = 20) uniform mat4 bone_xform[MAX_BONES];


layout (location = 0) in vec3 pos_attrib;                                             
layout (location = 1) in vec2 tex_coord_attrib;                                             
layout (location = 2) in vec3 normal_attrib;                                               
layout (location = 3) in ivec4 bone_id_attrib;
layout (location = 4) in vec4 weight_attrib;

out VertexData
{
   vec2 tex_coord;
   vec3 pw;   //world-space vertex position
   vec3 nw;   //world-space normal vector
	float w_debug;
} outData; 

void main(void)
{
	mat4 Skinning = mat4(1.0);

	if(num_bones > 0)
	{
		//Linear blend skinning
		Skinning	= bone_xform[bone_id_attrib[0]] * weight_attrib[0];
		Skinning	+= bone_xform[bone_id_attrib[1]] * weight_attrib[1];
		Skinning	+= bone_xform[bone_id_attrib[2]] * weight_attrib[2];
		Skinning	+= bone_xform[bone_id_attrib[3]] * weight_attrib[3];
	}

	//for debug visualization of bone weights
	outData.w_debug = 0.0;
	for(int i=0; i<4; i++)
	{
		if(bone_id_attrib[i] == debug_id)
		{
			outData.w_debug = weight_attrib[i];
		}
	}

	vec4 anim_pos = Skinning * vec4(pos_attrib, 1.0);

	if(Mode > 0)
	{
		gl_Position  = PV*M * anim_pos;
		outData.pw = vec3(M*anim_pos);

		vec4 anim_normal = Skinning * vec4(normal_attrib, 0.0);
		outData.nw      = vec3(M * anim_normal);
	}
	else //show mesh in rest pose
	{
		gl_Position  = PV*M * vec4(pos_attrib, 1.0);
		outData.pw = vec3(M*vec4(pos_attrib, 1.0));
		outData.nw   = vec3(M * vec4(normal_attrib, 0.0));
    }
	
   outData.tex_coord = vec2(tex_coord_attrib.s, 1.0-tex_coord_attrib.t); //tex coords flipped in the dae file
  	
}