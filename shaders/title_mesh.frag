#version 450

layout(location = 2) uniform vec4 color;

out vec4 fragcolor;//the output color for this fragment

void main(void)
{
    fragcolor = color;
}

