#version 450
#include "std_uniforms.h.glsl"
#line 4


out VertexData
{
	vec4 pobj;	//object-space vertex position
    vec2 uv;
} outData; 

layout(location=0) uniform vec4 mPlane = vec4(0.0, 0.0, 1.0, 0.0);
layout(location=2) uniform float mHmdDist = 1.0;

mat3 rotate(vec3 axis, float angle);

const vec4 quad[4] = vec4[] (vec4(-1.0, 1.0, 0.0, 1.0), 
							vec4(-1.0, -1.0, 0.0, 1.0), 
							vec4( 1.0, 1.0, 0.0, 1.0), 
							vec4( 1.0, -1.0, 0.0, 1.0) );

const vec3 n = vec3(0.0, 0.0, 1.0); //normal to the quad

void main(void)
{
    vec4 q = quad[gl_VertexID];
    vec2 hmd_flip = vec2(sign(mHmdDist), 1.0);
    outData.uv = 0.5*(hmd_flip*q.xy + vec2(1.0));

    vec3 axis = cross(mPlane.xyz, n);
    float sa = length(axis);
    if(sa<1e-8) axis = n;
    float angle = atan(sa, dot(mPlane.xyz, n));

    mat3 R = rotate(normalize(axis), angle);
    q.z -= mPlane.w;
    q.xyz = R*q.xyz;

	gl_Position = SceneUniforms.PV*SceneUniforms.M*q;
	outData.pobj = q;
}

//Rodrigues' formula
mat3 rotate(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.y * axis.x - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.z * axis.y - axis.x * s,
                oc * axis.x * axis.z - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c         );
}