#version 450
#include "std_uniforms.h.glsl"
#include "picking.h.glsl"
#include "xpbd_cs.h.glsl"
#line 6
 
layout(location = 1) uniform float time;
layout(location = 2) uniform int pass;

layout(binding = 2) uniform sampler2DShadow shadow_map;
layout(location = 10) uniform mat4 Shadow; //shadow matrix

in VertexData
{
	flat int constraint_id;
	flat uint flags;
	flat vec3 nw;
	vec3 pw;		//world-space vertex position
	vec3 peye;		//eye-space position
} inData;  

layout(location=0) out vec4 fragcolor; //the output color for this fragment    


const int mParticlePickId = 1;
const int mDistConstraintPickId = 2;
const int mBendConstraintPickId = 3;
const int mLinearConstraintPickId = 4;
const int mRotaryActuatorPickId = 5;
const int mLinearActuatorPickId = 6;
const int mAreaConstraintPickId = 7;

const vec4 ConColors[8] = vec4[](vec4(0.0), //none
			vec4(1.0), //part
			vec4(0.75, 0.75, 0.0, 1.0), //dist
			vec4(0.25, 0.5, 1.0, 1.0), //bend
			vec4(1.0, 0.25, 0.25, 1.0), //lin
			vec4(0.0, 1.0, 0.5, 1.0), //rotary act
			vec4(0.0, 0.75, 0.0, 1.0), //linear act
			vec4(0.75, 0.75, 0.0, 1.0) //linear act
			);

vec4 phong(vec4 ka, vec4 kd);

void main(void)
{   
	vec4 color = vec4(0.7);
	if(PickUniforms.pick_id >=0 && PickUniforms.pick_id <= mLinearActuatorPickId)
	{
		color = ConColors[PickUniforms.pick_id];
	}

	color = MouseoverColor(inData.constraint_id, color);

	if(IsSelected(inData.flags))
	{
		color.rgb = mix(color.rgb, vec3(1.0, 1.0, 0.0), 0.5);
	}

	fragcolor = phong(color, color);
	//fragcolor.rgb = abs(inData.nw);
	//fragcolor.rgb = phong().rgb;

	PickPassOutput(inData.pw.xyz, inData.constraint_id);
}

uniform float offset = 0.002;

//no texture, no attenuation, world-space phong lighting model
vec4 phong(vec4 ka, vec4 kd)
{
	//Compute per-fragment Phong lighting

	vec4 ambient_term = ka*LightUniforms.La;

	vec3 nw = normalize(inData.nw);			//world-space unit normal vector
	if(gl_FrontFacing==false)
	{
		nw = -nw;
	}

	vec4 shadow_coord = Shadow*vec4(inData.peye, 1.0);
    shadow_coord.z -= offset; //polygon offset
    float lit = textureProj(shadow_map, shadow_coord);

	vec3 lw = normalize(LightUniforms.light_w.xyz - inData.pw.xyz);	//world-space unit light vector
	vec4 diffuse_term = kd*LightUniforms.Ld*max(0.0, dot(nw, lw));

	vec3 vw = normalize(SceneUniforms.eye_w.xyz - inData.pw.xyz);	//world-space unit view vector
	vec3 rw = reflect(-lw, nw);	//world-space unit reflection vector

	vec4 specular_term = MaterialUniforms.ks*LightUniforms.Ls*pow(max(0.0, dot(rw, vw)), MaterialUniforms.shininess);

	return ambient_term + lit*(diffuse_term + specular_term);
}














