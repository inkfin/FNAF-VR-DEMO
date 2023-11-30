#version 450

layout(location = 0) uniform mat4 PV;
layout(location = 1) uniform mat4 M;

layout(location = 0) in vec3 pos_attrib;
layout(location = 1) in vec2 tex_coord_attrib;
layout(location = 2) in vec3 normal_attrib;

out VertexData
{
    vec2 tex_coord;
    vec3 pw;//world-space vertex position
    vec3 nw;//world-space normal vector
} outData;

void main(void)
{
    gl_Position = PV * M * vec4(pos_attrib, 1.0);
    outData.pw  = vec3(M * vec4(pos_attrib, 1.0));
    outData.nw  = vec3(M * vec4(normal_attrib, 0.0));

    outData.tex_coord = vec2(tex_coord_attrib.s, 1.0-tex_coord_attrib.t);//tex coords flipped in the dae file
}