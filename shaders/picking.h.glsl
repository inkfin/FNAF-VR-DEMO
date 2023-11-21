
const int kPickOutLoc = 7;
const int kPickUniformsBinding = 50;
const int kLeftPickImageBinding = 6;
const int kRightPickImageBinding = 7;

layout (location = kPickOutLoc) out vec4 pick;

layout(std140, binding = kPickUniformsBinding) uniform PickData
{
	ivec4 mouseover;	//type left, id left, type right, id right
	ivec4 picked;		//type left, id left, type right, id right
	int pick_pass;
	int pick_id;
} PickUniforms;

layout(rgba32f, binding = kLeftPickImageBinding) restrict readonly uniform image2D uLeftPickImage; 
layout(rgba32f, binding = kRightPickImageBinding) restrict readonly uniform image2D uRightPickImage;

//pack 4 half-precision floats into vec2
vec2 packHalf4x16(in vec4 v)
{
	uint uxy = packHalf2x16(v.xy);
	uint uzw = packHalf2x16(v.zw);
	return vec2(uintBitsToFloat(uxy), uintBitsToFloat(uzw));
}

//unpack 4 half-precision floats from vec2
vec4 unpackHalf4x16(in vec2 v)
{
	uint uxy = floatBitsToUint(v.x);
	uint uzw = floatBitsToUint(v.y);
	return vec4(unpackHalf2x16(uxy), unpackHalf2x16(uzw));
}

ivec2 GetLeftPick(inout vec3 pos)
{
	vec4 im = imageLoad(uLeftPickImage, ivec2(0));
	pos = unpackHalf4x16(im.xy).xyz;
	return ivec2(floatBitsToInt(im.z), floatBitsToInt(im.w));
}

ivec2 GetRightPick(inout vec3 pos)
{
	vec4 im = imageLoad(uRightPickImage, ivec2(0));
	pos = unpackHalf4x16(im.xy).xyz;
	return ivec2(floatBitsToInt(im.z), floatBitsToInt(im.w));
}

void PickPassOutput(vec3 pos, int sub_id)
{
	if(PickUniforms.pick_pass != 0)
	{
		vec2 hpos = packHalf4x16(vec4(pos, 1.0));
		pick = vec4(hpos, intBitsToFloat(PickUniforms.pick_id), intBitsToFloat(sub_id));
	}
}

vec4 MouseoverColor(vec4 color)
{
	vec4 color_out = color;
	if(PickUniforms.pick_id == PickUniforms.mouseover.x || PickUniforms.pick_id == PickUniforms.mouseover.z)
	{
		const vec4 highlight = vec4(1.0, 0.0, 0.0, 1.0);
		color_out = mix(color, highlight, 0.5);
	}
	return color_out;
}

vec4 MouseoverColor(int sub_id, vec4 color)
{
	vec4 color_out = color;
	if((PickUniforms.mouseover.x != -1 && PickUniforms.pick_id == PickUniforms.mouseover.x && sub_id == PickUniforms.mouseover.y) || 
	   (PickUniforms.mouseover.z != -1 && PickUniforms.pick_id == PickUniforms.mouseover.z && sub_id == PickUniforms.mouseover.w))
	{
		const vec4 highlight = vec4(1.0, 0.0, 0.0, 1.0);
		color_out = mix(color, highlight, 0.75);
	}
	return color_out;
}