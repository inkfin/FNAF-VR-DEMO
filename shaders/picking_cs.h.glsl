const int kPickOutLoc = 7;
const int kPickUniformsBinding = 50;

layout (location = kPickOutLoc) out vec4 pick_out;

layout(std140, binding = kPickUniformsBinding) uniform PickData
{
	ivec4 mouseover;
    ivec4 picked;
	int pick_pass;
	int pick_id;
} PickUniforms;

void PickPassOutput(int sub_id)
{
	if(PickUniforms.pick_pass > 0)
	{
		pick_out = vec4(intBitsToFloat(PickUniforms.pick_id), intBitsToFloat(sub_id), 1.0, 1.0);
	}
}

void PickPassOutput()
{
	if(PickUniforms.pick_pass > 0)
	{
		pick_out = vec4(intBitsToFloat(PickUniforms.pick_id), 0.0, 1.0, 1.0);
	}
}

vec4 MouseoverColor(vec4 color)
{
	vec4 color_out = color;
	if(PickUniforms.pick_id == PickUniforms.mouseover.x)
	{
		const vec4 highlight = vec4(1.0, 0.0, 0.0, 1.0);
		color_out = mix(color, highlight, 0.5);
	}
	return color_out;
}

vec4 MouseoverColor(int sub_id, vec4 color)
{
	vec4 color_out = color;
	if(PickUniforms.mouseover.x != -1 && PickUniforms.pick_id == PickUniforms.mouseover.x && sub_id == PickUniforms.mouseover.y)
	{
		const vec4 highlight = vec4(1.0, 0.0, 0.0, 1.0);
		color_out = mix(color, highlight, 0.5);
	}
	return color_out;
}

vec4 PickedColor(vec4 color)
{
	vec4 color_out = color;
	if(PickUniforms.pick_id == PickUniforms.picked.x)
	{
		const vec4 highlight = vec4(0.0, 1.0, 0.0, 1.0);
		color_out = mix(color, highlight, 0.5);
	}
	return color_out;
}

vec4 PickedColor(int sub_id, vec4 color)
{
	vec4 color_out = color;
	if(PickUniforms.picked.y == sub_id && PickUniforms.pick_id == PickUniforms.picked.x)
	{
		const vec4 highlight = vec4(1.0, 1.0, 0.0, 1.0);
		color_out = mix(color, highlight, 0.5);
	}
	return color_out;
}