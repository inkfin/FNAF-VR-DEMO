#version 450
#include "std_uniforms.h.glsl"
#include "picking.h.glsl"
#include "xpbd_cs.h.glsl"
#line 6
 
layout(location = 1) uniform float time;
layout(location = 2) uniform int pass;

layout(binding = 2) uniform sampler2DShadow shadow_map;
layout(location = 10) uniform mat4 Shadow; //shadow matrix

in GeomData
{
	vec3 pe;
	vec3 a;
	vec3 b;
	float hw;
	flat int constraint_id;
	flat uint flags;
} inData;  

layout(location=0) out vec4 fragcolor; //the output color for this fragment    

// https://iquilezles.org/articles/distfunctions
float sdCapsule( vec3 p, vec3 a, vec3 b, float r )
{
	vec3 pa = p-a, ba = b-a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return length( pa - ba*h ) - r;
}

vec2 nearestLineSeg(vec2 p, vec2 a, vec2 b)
{
	const float eps = 1e-8;
	vec2 dir = b-a;
    float dSqr = dot(dir,dir);
    if(dSqr <= eps) return a;
    
    float t = clamp(dot(p - a, b - a) / dSqr, 0.0, 1.0);
    vec2 j = a + t * (b - a);
    
    return j;
}

const int mParticlePickId = 1;
const int mDistConstraintPickId = 2;
const int mBendConstraintPickId = 3;
const int mLinearConstraintPickId = 4;
const int mRotaryActuatorPickId = 5;
const int mLinearActuatorPickId = 6;

const vec4 ConColors[7] = vec4[](vec4(0.0), //none
			vec4(1.0), //part
			vec4(0.75, 0.75, 0.0, 1.0), //dist
			vec4(0.0, 0.25, 1.0, 1.0), //bend
			vec4(1.0, 0.25, 0.25, 1.0), //lin
			vec4(0.0, 1.0, 0.5, 1.0), //rotary act
			vec4(0.0, 0.75, 0.0, 1.0) //linear act
			);

uniform float offset = 0.002;

void main(void)
{   
	float d = sdCapsule(inData.pe, inData.a, inData.b, inData.hw);
	if(d>0.0) discard;

	vec3 ne;
	vec2 near = nearestLineSeg(inData.pe.xy, inData.a.xy, inData.b.xy);
	ne.xy = inData.pe.xy-near.xy;
	ne.z = sqrt(inData.hw*inData.hw-dot(ne.xy, ne.xy));

	vec4 peye = vec4(inData.pe.xy, inData.pe.z+ne.z, 1.0);
	ne = normalize(ne);

	vec4 L_eye = SceneUniforms.V*LightUniforms.light_w;
	vec3 le = normalize(L_eye.xyz - peye.xyz);

	vec4 color = vec4(0.7);
	if(PickUniforms.pick_id >=0 && PickUniforms.pick_id <= mLinearActuatorPickId)
	{
		color = ConColors[PickUniforms.pick_id];
	}

    vec4 shadow_coord = Shadow*peye;
    shadow_coord.z -= offset; //polygon offset
    float lit = textureProj(shadow_map, shadow_coord);

	vec4 ambient_term = vec4(color.rgb, 1.0)*LightUniforms.La;
	vec4 diffuse_term = vec4(color.rgb, 1.0)*LightUniforms.Ld*max(0.0, dot(ne, le));

	vec3 ve = -normalize(peye.xyz);
	vec3 re = reflect(-le, ne);
	vec4 specular_term = MaterialUniforms.ks*LightUniforms.Ls*pow(max(0.0, dot(re, ve)), MaterialUniforms.shininess);

	fragcolor = ambient_term + lit*(diffuse_term + specular_term);
	fragcolor = MouseoverColor(inData.constraint_id, fragcolor);

	if(IsSelected(inData.flags))
	{
		fragcolor.rgb = mix(fragcolor.rgb, vec3(1.0, 1.0, 0.0), 0.5);
	}

	vec4 p_ndc = SceneUniforms.P*peye;
	p_ndc = p_ndc/p_ndc.w;
    gl_FragDepth = 0.5*(gl_DepthRange.diff*p_ndc.z + gl_DepthRange.near + gl_DepthRange.far);

	//fragcolor.rgb = vec3(abs(nw));
	//fragcolor.rgb = vec3(lit);

	vec4 pw = inverse(SceneUniforms.V)*peye;
	PickPassOutput(pw.xyz, inData.constraint_id);
}




