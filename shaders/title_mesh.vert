#version 450

layout(location = 0) uniform mat4 PV;
layout(location = 1) uniform mat4 M;

layout(location = 0) in vec3 pos_attrib;

void main(void)
{
    gl_Position = PV * M * vec4(pos_attrib, 1.0);
}