const int kBrushesUniformsBinding = 64;

layout(std140, binding = kBrushesUniformsBinding) uniform BrushData
{
	mat4 mLocalM[2];
	vec4 mLocalPos[2];
	vec4 mSize[2];
	vec4 mColor[2];
	vec4 mPressure;
} BrushUniforms;

