#version 450  

#include "std_uniforms.h.glsl"
#include "picking.h.glsl"
#line 6

const int LINE_MODE = 0;
const int POINT_MODE = 1;
const int QUAD_MODE = 2; //Draw thick lines as a quad

layout(location = 0) uniform mat4 M;
layout(location = 50) uniform int hand;
layout(location = 51) uniform int mode = LINE_MODE;
layout(location = 52) uniform vec3 pos = vec3(0.0);
layout(location = 53) uniform float len = 30.0;

vec4 line_verts[] = vec4[2](vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 0.0, -1.0, 1.0));
float ends[] = float[2](0.0, 1.0);

out VertexData
{
    float end;

} outData; 

void main(void)
{
    if(mode == LINE_MODE)
    {
        vec4 p = vec4(line_verts[gl_VertexID].xyz*len, 1.0);
        outData.end = ends[gl_VertexID];
        gl_Position = SceneUniforms.PV*M*p;
    }
    if(mode == QUAD_MODE)
    {
        vec4 clip[2];
        clip[0] = SceneUniforms.PV*M*vec4(line_verts[0].xyz, 1.0);
        clip[1] = SceneUniforms.PV*M*vec4(line_verts[1].xyz*len, 1.0);

        vec3 pw;
        ivec2 left_pick_id = GetLeftPick(pw);
        if(hand==0 && (left_pick_id[0] > 0) && distance(line_verts[0].xyz, pw)<len)
        {
            clip[1] = SceneUniforms.PV*vec4(pw, 1.0);
        }
        ivec2 right_pick_id = GetRightPick(pw);
        if(hand==1 && (right_pick_id[0] > 0) && distance(line_verts[0].xyz, pw)<len)
        {
            clip[1] = SceneUniforms.PV*vec4(pw, 1.0);
        }
        
      
        vec2 ndc[2];
        ndc[0] = clip[0].xy/clip[0].w;
        ndc[1] = clip[1].xy/clip[1].w;  

        vec2 ndc_dir = normalize(ndc[1]-ndc[0]);
        vec2 n = vec2(-ndc_dir.y, ndc_dir.x);

        float dir = 0.0035*float(2*(gl_VertexID%2)-1);
        vec4 offset = vec4(n*dir, 0.0, 0.0);
        gl_Position = clip[gl_VertexID/2]+offset;
        outData.end = ends[gl_VertexID/2];
    }
    if(mode == POINT_MODE)
    {
        gl_Position = SceneUniforms.PV*vec4(pos, 1.0);
        gl_Position.z -= 0.002; //fake polygon offset
        gl_PointSize = 15.0;
    }
}

