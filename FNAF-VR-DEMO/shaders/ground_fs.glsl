#version 450

#include "std_uniforms.h.glsl"
#include "picking.h.glsl"
#line 6


const int BRUSH_MODE = 0;
const int TRANSFORM_MODE = 1;

layout(location = 30) uniform sampler2D Diffuse_tex;
layout(location=0) uniform mat4 M_voxel;
layout(location=1) uniform vec3 L_pointer;
layout(location=2) uniform vec3 R_pointer;
layout(location=3) uniform int uMode = BRUSH_MODE;

in VertexData
{
   vec2 tex_coord;
   vec3 pw;			//world-space vertex position
   vec3 nw;			//world-space normal vector
   vec3 peye;		//eye-space position
} inData; 

out vec4 fragcolor;

const float PI = 3.1515926535;

float sdBox( in vec2 p, in vec2 b );
float sdCircle( in vec2 p, in float r );
float polar_grid(vec2);
float rotation_ring(vec2);
float voxel_outline(vec2);
float translate(vec2);
vec2 pointers(); 

void main(void)
{  
    const float eps = 0.001;
    vec4 k_bg = vec4(0.9);
    vec4 k_grid = vec4(0.25, 0.25, 1.0, 1.0);
    vec4 k_vox = vec4(1.0, 0.25, 0.25, 1.0);
    vec4 k_pointer = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 k_ring = vec4(0.0, 0.0, 1.0, 1.0);
    vec4 k_trans = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k_mo = vec4(0.1);

    //vec2 pw2 = inData.pw.xz;
    vec2 pw2 = (M_voxel*vec4(inData.pw.xyz, 1.0)).xz;
    fragcolor = k_bg;

    float d_grid = polar_grid(pw2);
    fragcolor = mix(k_grid, k_bg, d_grid);

    //vec3 lw = LightUniforms.light_w.xyz-inData.pw.xyz;
    //float diff = max(0.0, dot(inData.nw, lw));
    //fragcolor = vec4(diff);

    float r = length(pw2);
    fragcolor = mix(fragcolor, k_grid, smoothstep(15.0, 30.0, r));

    //vec2 d_pointer = pointers();
    //fragcolor = mix(fragcolor, k_pointer, 1.0-smoothstep(0.0, 0.001, d_pointer.x));
    //fragcolor = mix(fragcolor, k_pointer, 1.0-smoothstep(0.0, 0.001, d_pointer.y));

    vec3 p_left, p_right;
    ivec2 id_left = GetLeftPick(p_left);
    ivec2 id_right = GetRightPick(p_right);
    int sub_id_left = id_left[1];
    int sub_id_right = id_right[1];

    int sub_id = 0;

    if(uMode==TRANSFORM_MODE)
    {
        if(sub_id_left==1 || sub_id_right==1)
        {
            k_ring += k_mo;
        }

        if(sub_id_left==2 || sub_id_right==2)
        {
            k_vox += k_mo;
        }

        if(sub_id_left==3 || sub_id_right==3)
        {
            k_trans += k_mo;
        }

        float d_ring = rotation_ring(pw2);
        fragcolor = mix(fragcolor, k_ring, smoothstep(eps, 0.0, d_ring));
        if(d_ring < 0.0) sub_id = 1;

        float d_trans = translate(pw2);
        fragcolor = mix(fragcolor, k_trans, smoothstep(eps, 0.0, d_trans));
        if(d_trans < 0.0) sub_id = 3;
    }

    float d_vox = voxel_outline(pw2);
    fragcolor = mix(fragcolor, k_vox, smoothstep(eps, 0.0, d_vox));
    if(d_vox < 0.0) sub_id = 2;

    const float r_pointer = 0.01;

    if(int(id_left)==1)
    {
        float d = distance(p_left.xyz, inData.pw) - r_pointer;
        fragcolor = mix(fragcolor, k_pointer, smoothstep(eps, 0.0, d));
    }
    if(int(id_right)==1)
    {
        float d = distance(p_right.xyz, inData.pw) - r_pointer;
        fragcolor = mix(fragcolor, k_pointer, smoothstep(eps, 0.0, d));
    }

    PickPassOutput(inData.pw, sub_id);
}

float polar_grid(vec2 p)
{
    float r = length(p);
    float theta = atan(p.y, p.x);

    //float rep_r = abs(fract(r - 0.1*SceneUniforms.Time)-0.5);
    float rep_r = abs(fract(r)-0.5);
    float rep_theta = abs(fract(9.0*theta/PI)-0.25);

    float dr = 0.002*r;
    float fr = smoothstep(0.0, dr, rep_r);

    float dtheta = 0.01/(r+1.01);
    float ftheta = smoothstep(0.001, 0.001+dtheta, rep_theta);
    float f = min(fr, ftheta);
    return f;
}

float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float sdCircle( in vec2 p, in float r ) 
{
    return length(p)-r;
}

float sdEquilateralTriangle( in vec2 p )
{
    const float k = sqrt(3.0);
    p.x = abs(p.x) - 1.0;
    p.y = p.y + 1.0/k;
    if( p.x+k*p.y>0.0 ) p = vec2(p.x-k*p.y,-k*p.x-p.y)/2.0;
    p.x -= clamp( p.x, -2.0, 0.0 );
    return -length(p)*sign(p.y);
}

float opUnion( float d1, float d2 ) { return min(d1,d2); }
float opSubtraction( float d1, float d2 ) { return max(-d1,d2); }
float opIntersection( float d1, float d2 ) { return max(d1,d2); }

float voxel_outline(vec2 p)
{
    float d = sdBox(p, vec2(0.5, 0.5));
    d = abs(d)-0.05;
    return d;
}

vec2 pointers()
{
    float r = 0.01;
    vec2 d;
    d.x = distance(vec3(1.0, 1.0, 1.0)*L_pointer.xyz, inData.pw) - r;
    d.y = distance(vec3(1.0, 1.0, 1.0)*R_pointer.xyz, inData.pw) - r;
    return d;
}

float rotation_ring(vec2 p)
{
   const float r_outer = 0.8;
   const float r_inner = 0.7;
   float d_outer = sdCircle(p, r_outer);
   float d_inner = sdCircle(p, r_inner);
   return opSubtraction(d_inner, d_outer);
   //return d_inner;
}

float translate(vec2 p)
{
    const float s = 6.0;
    float d1 = sdEquilateralTriangle(s*(abs(p)-vec2(0.0, 0.6)));
    float d2 = sdEquilateralTriangle(s*(abs(p.yx)-vec2(0.0, 0.6)));
    return opUnion(d1, d2);
}













