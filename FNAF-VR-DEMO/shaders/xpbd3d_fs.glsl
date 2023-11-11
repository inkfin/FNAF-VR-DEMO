#version 450
#include "std_uniforms.h.glsl"
#include "picking.h.glsl"
#line 4
 
layout(location = 1) uniform float time;
layout(location = 2) uniform int pass;
layout(binding = 1) uniform samplerCube skybox_tex;

layout(binding = 2) uniform sampler2DShadow shadow_map;
layout(location = 10) uniform mat4 Shadow; //shadow matrix

uniform float alpha_scale = 1.0;

in VertexData
{
	vec3 pw;		//world-space vertex position
	vec3 peye;		//eye-space position
	vec3 color;
	float radius;
    float w;
	flat int vertex_id;
} inData;   

out vec4 fragcolor; //the output color for this fragment    

vec3 mixstep(vec3 c0, vec3 c1, float e0, float e1, float x)
{
    return mix(c0, c1, smoothstep(e0, e1, x));
}

vec4 mixstep(vec4 c0, vec4 c1, float e0, float e1, float x)
{
    return mix(c0, c1, smoothstep(e0, e1, x));
}

void sphere();
float eps = 1e-6;
void main(void)
{   
	sphere();
}

uniform float offset = 0.002;

void sphere()
{
    float r_world = inData.radius;
    
    vec2 coord = vec2(1.0, -1.0)*(2.0*gl_PointCoord.xy - vec2(1.0));
    float r_coord = length(coord);
    if(r_coord>=1.0) discard;
    float alpha = 1.0;
    vec4 p_local = vec4(coord*r_world, 0.0, 1.0);
    p_local.z = sqrt(r_world*r_world - dot(p_local.xy, p_local.xy));

    vec4 p_eye =  p_local + vec4(inData.peye, 0.0);
    vec4 p_clip = SceneUniforms.P*p_eye;
    vec4 p_ndc = p_clip/p_clip.w;
    gl_FragDepth = 0.5*(gl_DepthRange.diff*p_ndc.z + gl_DepthRange.near + gl_DepthRange.far);

	vec3 ne;
	ne.xy = vec2(1.0, -1.0)*(2.0*gl_PointCoord.xy-vec2(1.0));
	ne.z = sqrt(1.0-dot(ne.xy, ne.xy));

    vec4 shadow_coord = Shadow*p_eye;
    shadow_coord.z -= offset; //polygon offset
    float lit = textureProj(shadow_map, shadow_coord);

    //Eye-space lighting
    vec4 L_eye = SceneUniforms.V*LightUniforms.light_w;
	vec3 le = normalize(L_eye.xyz - inData.peye.xyz);

    //lighting
	vec4 ambient_term = vec4(inData.color, 1.0)*LightUniforms.La;
	vec4 diffuse_term = vec4(inData.color, 1.0)*LightUniforms.Ld*max(0.0, dot(ne, le));

    if(inData.w <= 0.0 && r_coord<0.5)
	{
		ambient_term *= 0.5;
		diffuse_term *= 0.5;
	}

	vec3 ve = -normalize(inData.peye.xyz);
	vec3 re = reflect(-le, ne);

	vec4 specular_term = MaterialUniforms.ks*LightUniforms.Ls*pow(max(0.0, dot(re, ve)), MaterialUniforms.shininess);
    float rim = pow(p_local.z/r_world, 1.5);
    
	fragcolor = rim*ambient_term + lit*(diffuse_term + specular_term);
    
    //gamma
    fragcolor = pow(fragcolor, vec4(1.0/2.2));
    fragcolor.a = alpha;

    fragcolor = MouseoverColor(inData.vertex_id, fragcolor); 

    //fragcolor.xyz = ne.xyz; 

    //PickPassOutput(inData.vertex_id);
    PickPassOutput(inData.pw, inData.vertex_id);
}













