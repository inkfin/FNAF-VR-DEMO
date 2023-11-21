#version 430

const int LINE_MODE = 0;
const int POINT_MODE = 1;
const int QUAD_MODE = 2; //Draw thick lines as a quad

const int SOLID_STYLE = 0;
const int DOTTED_STYLE = 1;

layout(location = 50) uniform int hand;
layout(location = 51) uniform int mode = LINE_MODE;
layout(location = 54) uniform vec3 hand_colors[2] = vec3[2](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
layout(location = 56) uniform int style = SOLID_STYLE;

out vec4 fragcolor;           

in VertexData
{
   float end;

} inData; 
      
void main(void)
{   
    if(mode == LINE_MODE || mode == QUAD_MODE)
    {
        //vec3 color = inData.end*hand_colors[hand];
        vec3 color = hand_colors[hand];

        if(style ==  DOTTED_STYLE)
        {
            float dots = fract(100.0*inData.end)-0.5;
            if(dots < 0.0) discard;
        }

        fragcolor = vec4(color, 0.5);
        return;
    }
    if(mode == POINT_MODE)
    {
        vec3 color = hand_colors[hand];
        vec2 p = 2.0*gl_PointCoord-vec2(1.0);
        if(dot(p,p) >= 1.0) discard;
        float a = 1.0-smoothstep(0.9, 0.999, dot(p,p));
        fragcolor = vec4(color*a, a);
        return;
    }
    fragcolor = vec4(1.0);
}




















