#version 450

layout(local_size_x = 10, local_size_y = 10, local_size_z = 10) in;

const int MODE_MIN = 0;
const int MODE_MAX = 1;
const int MODE_AVG = 2;
const int MODE_MAX_RECT = 3;

layout(location=0) uniform int uMode = MODE_MIN;
layout(location=1) uniform ivec3 offset = ivec3(0);

layout(rgba8, binding = 0) readonly uniform image3D input_image;
layout(rgba8, binding = 1) writeonly uniform image3D output_image;

struct RaytraceData
{
	vec4 ray_intersection[2]; // [2] for left/right controller
	ivec4 dirty_rect_min[2];
	ivec4 dirty_rect_max[2];
};

layout (std430, binding = 0) restrict buffer RT_DATA 
{
	RaytraceData mRaytraceData;
};

layout(location=14) uniform int hand = -1;


void main()
{
	ivec3 gid = ivec3(gl_GlobalInvocationID.xyz);
	ivec3 in_size = imageSize(input_image);
	ivec3 out_size = imageSize(output_image);
	ivec3 max_coord = in_size-ivec3(1);

	if(any(greaterThanEqual(gid+offset, out_size))) return;

	vec4 v[2][2][2];
	for(int i=0; i<2; i++)
	for(int j=0; j<2; j++)
	for(int k=0; k<2; k++)
	{
		ivec3 in_coord = clamp(2*(gid+offset)+ivec3(i,j,k), ivec3(0), max_coord);
		v[i][j][k] = imageLoad(input_image, in_coord);
	}

	vec4 vout;
	if(uMode == MODE_MAX || uMode == MODE_MAX_RECT)
	{
		vout = max(max(max(v[0][0][0], v[1][0][0]), max(v[0][1][0], v[1][1][0])), max(max(v[0][0][1], v[1][0][1]), max(v[0][1][1], v[1][1][1])));
	}
	else if(uMode == MODE_MIN)
	{
		vout = min(min(min(v[0][0][0], v[1][0][0]), min(v[0][1][0], v[1][1][0])), min(min(v[0][0][1], v[1][0][1]), min(v[0][1][1], v[1][1][1])));
	}
	else if(uMode == MODE_AVG)
	{
		vout = vec4(0.0);
		for(int i=0; i<2; i++)
		for(int j=0; j<2; j++)
		for(int k=0; k<2; k++)
		{
			vout += v[i][j][k];
		}
		vout *= 0.125;
	}
	else if(uMode == MODE_MAX_RECT)
	{
		
		vout = max(max(max(v[0][0][0], v[1][0][0]), max(v[0][1][0], v[1][1][0])), max(max(v[0][0][1], v[1][0][1]), max(v[0][1][1], v[1][1][1])));
	}

	ivec3 out_coord = clamp(gid+offset, ivec3(0), out_size-ivec3(1));
	imageStore(output_image, out_coord, vout);
}

