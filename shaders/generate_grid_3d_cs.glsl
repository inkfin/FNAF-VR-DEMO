#version 450
#include "std_uniforms.h.glsl"
#include "brushes_cs.h.glsl"
#line 5
layout(local_size_x = 10, local_size_y = 10, local_size_z = 10) in;

layout(rgba8, binding = 0) uniform image3D uImage; 
layout(binding = 1) uniform sampler3D uGrid; 

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


const int STATIC_MODE = 0;
//const int ANIMATE_MODE = 1;
const int ADD_MODE = 2;
const int SUB_MODE = 3; //TODO debug
const int PAINT_MODE = 4;
const int RAYCAST_ADD_MODE = 5;
const int RAYCAST_SUB_MODE = 6;
const int RAYCAST_PAINT_MODE = 7;

uniform int uMode = STATIC_MODE;

layout(location=10) uniform vec3 brush_pos = vec3(0.0);
layout(location=11) uniform float brush_r = 1.0f;
layout(location=12) uniform vec4 brush_color = vec4(1.0f);
layout(location=13) uniform ivec3 offset = ivec3(0);
layout(location=14) uniform int hand = -1;
layout(location=15) uniform mat3 brush_xform = mat3(1.0);

const int SPHERE_SHAPE = 0;
const int CUBE_SHAPE = 1;
layout(location=16) uniform int brush_shape = SPHERE_SHAPE;

layout(location=31) uniform vec3 brush_ro;
layout(location=32) uniform vec3 brush_rd;

void add(ivec3 coord);
void sub(ivec3 coord);
void paint(ivec3 coord);
void raycast_add(ivec3 coord);
void raycast_sub(ivec3 coord);
void raycast_paint(ivec3 coord);
float sdSphere( vec3 p, float s );

void main()
{
	ivec3 gid = ivec3(gl_GlobalInvocationID.xyz);
	ivec3 size = imageSize(uImage);

	if(any(greaterThanEqual(gid, size))) return;

	if(uMode == STATIC_MODE)
	{
		vec3 cen = 0.5*vec3(size);
		float r = 0.1*size.x;
		/*
		float f = 0.0;
		if(distance(gid, cen) < r)
		{
			f = 1.0;
		}
		*/

		float f = -sdSphere(vec3(gid)-cen, r);

		//uint cu32 = packUnorm4x8(brush_color);
		//float cf32 = uintBitsToFloat(cu32);
		imageStore(uImage, gid, vec4(brush_color.rgb, f));
		return;
	}
	else if(uMode==ADD_MODE)
	{
		add(gid);
		return;
	}
	else if(uMode==SUB_MODE)
	{
		sub(gid);
		return;
	}
	else if(uMode==PAINT_MODE)
	{
		paint(gid);
		return;
	}
	else if(uMode == RAYCAST_SUB_MODE)
	{
		raycast_sub(gid);
		return;
	}
	else if(uMode == RAYCAST_ADD_MODE)
	{
		raycast_add(gid);
		return;
	}
	else if(uMode == RAYCAST_PAINT_MODE)
	{
		raycast_paint(gid);
		return;
	}
}

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}

float sdEllipsoid( in vec3 p, in vec3 r ) 
{
    float k0 = length(p/r);
    float k1 = length(p/(r*r));
    return (k0<1.0) ? (k0-1.0)*min(min(r.x,r.y),r.z) : k0*(k0-1.0)/k1;
}

float sdBox( in vec3 p, in vec3 r ) 
{
  vec3 q = abs(p) - r/sqrt(2.0);
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}


float sdBrush( in vec3 p, in vec3 r ) 
{
	if(brush_shape == SPHERE_SHAPE)
	{
		return sdEllipsoid(p, r);
	}
	else if(brush_shape == CUBE_SHAPE)
	{
		return sdBox(p, r);
	}
}

void add(ivec3 coord)
{
	coord += offset;
	vec4 im = imageLoad(uImage, coord);
	const float p = BrushUniforms.mPressure[hand];
	float d1 = sdBrush(brush_xform*(vec3(coord)-brush_pos), p*BrushUniforms.mSize[hand].xyz);
	float d2 = im.w;

	if(d1<0.0)
	{
		float d = min(d1, d2);
		vec3 color = mix(im.rgb, BrushUniforms.mColor[hand].rgb, BrushUniforms.mColor[hand].a);
		imageStore(uImage, coord, vec4(color, d));
	}
}


void sub(ivec3 coord) //really ADD???
{
	coord += offset;
	vec4 im = imageLoad(uImage, coord);
	const float p = BrushUniforms.mPressure[hand];
	float d1 = sdBrush(brush_xform*(vec3(coord)-brush_pos), p*BrushUniforms.mSize[hand].xyz);
	float d2 = im.w;

	float d;
	if(d1<0.0)
	{
		d = max(-d1, d2);
		vec3 color = mix(im.rgb, BrushUniforms.mColor[hand].rgb, BrushUniforms.mColor[hand].a);
		imageStore(uImage, coord, vec4(color, d));	
	}
}


void paint(ivec3 coord)
{
	coord += offset;
	vec4 im = imageLoad(uImage, coord);
	const float p = BrushUniforms.mPressure[hand];
	float d = sdBrush(brush_xform*(vec3(coord)-brush_pos), p*BrushUniforms.mSize[hand].xyz);
	
	if(d <= 0.0)
	{
		vec3 color = mix(im.rgb, BrushUniforms.mColor[hand].rgb, p*BrushUniforms.mColor[hand].a);
		imageStore(uImage, coord, vec4(color.rgb,im.w));	
	}
}

struct RayHit
{
	vec3 p;
	vec3 n;
	ivec3 cell;
	float t_enter;
	float t_exit;
	int steps;
	bool hit;
	vec4 color;
};

RayHit CreateRayHit()
{
	RayHit r;
	r.hit = false;
	r.t_enter = -1;
	r.t_exit = -1;
	r.steps = 0;
	return r;
}

struct Ray
{
	vec3 ro;
	vec3 rd;
	vec3 m; //=1.0/rd
	float tmin;
	float tmax;
};

Ray CreateRay(vec3 ro, vec3 rd)
{
	//float m = clamp(1.0/rd, -1e10, 1e10);
	Ray ray = Ray(ro, rd, 1.0/rd, 0.0, 1e10);
	return ray;
}

Ray CreateRay(vec3 ro, vec3 rd, float tmin, float tmax)
{
	//float m = clamp(1.0/rd, -1e10, 1e10);
	Ray ray = Ray(ro, rd, 1.0/rd, tmin, tmax);
	return ray;
}

vec3 p_ray(in Ray ray, float t)
{
	return ray.ro + t*ray.rd;
}

vec3 sort3( vec3 c)
{
	c.rg = (c.r < c.g) ? c.rg : c.gr;
	c.rb = (c.r < c.b) ? c.rb : c.br;
	c.gb = (c.g < c.b) ? c.gb : c.bg;
	return c;
}

vec3 sort3( vec3 c, out ivec3 ix)
{
	//Can this be optimized?
	ix = ivec3(0, 1, 2);
	if(c.r > c.g)
	{
		c.rg = c.gr;
		ix.rg = ix.gr;
	}
	if(c.r > c.b)
	{
		c.rb = c.br;
		ix.rb = ix.br;
	}
	if(c.g > c.b)
	{
		c.gb = c.bg;
		ix.gb = ix.bg;
	}
	return c;
}


#include "intersect_cs.h.glsl"
#line 255


layout(location = 20) uniform int min_level = 0;
layout(location = 21) uniform int max_level = 9;

RayHit raytrace_grid_3d(in Ray ray)
{
	RayHit ray_hit = CreateRayHit();
	
	int init_level = max_level;
	int level = init_level;
	int scale = 1<<level;

	const ivec3 size = textureSize(uGrid, 0);
	const ivec3 min_cell = ivec3(0);
	const ivec3 max_cell = size-ivec3(1);
	const mat3 N = mat3(1.0); 

	ivec3 cell_step = ivec3(sign(ray.rd));
	ivec3 side = ivec3(step(vec3(0.0), ray.rd));
	
	const vec3 bbox_hw = 0.5*vec3(size);
	vec3 bbox_n; //returned from boxIntersector
	vec2 tNF = boxIntersector(ray.ro, ray.rd, bbox_hw, bbox_n);

	//Handle po inside grid bounding box
	vec3 po = p_ray(ray, ray.tmin);
	if(all(greaterThanEqual(po, vec3(0.0))) && all(lessThanEqual(po, vec3(size)))) 
	{
		tNF[0] = ray.tmin;	
	}

	float t = tNF[0];

	if(t<ray.tmin) //Box behind ray, or missed
	{	
		ray_hit.t_enter = t;
		return ray_hit; //ray_hit.hit is already set to false
	}

	ray_hit.p = p_ray(ray, t);
	ray_hit.n = bbox_n;

	//debug
	//ray_hit.hit = true;
	//return ray_hit;
	//debug

	ivec3 cell = ivec3(floor(ray_hit.p));
	cell = clamp(cell, min_cell, max_cell);
	
	int i;
	for(i=0; i<4000; i++)
	{
		vec4 f = texelFetch(uGrid, cell>>level, level); //0: empty space

		if(f.w == 0.0 && level < max_level)
		{
			vec4 f0 = texelFetch(uGrid, cell>>(level+1), level+1);
			if(f0.w == 0.0)
			{
				f = f0;
				level++;
				scale = 1<<level;
			}
		}

		if(f.w > 0.0 && level > min_level)
		{
			//ivec3 old_root = scale*(cell/scale);
			ivec3 old_root = (cell>>level)<<level;
			vec3 p = p_ray(ray, t);

			cell = ivec3(floor(p));
			cell = clamp(cell, old_root, old_root+ivec3(scale-1));

			level--;
			scale = 1<<level;
			
			continue;
		}

		if(f.w > 0.0 && level == min_level)
		{
			if(ray_hit.t_enter<0.0)
			{
				ray_hit.t_enter = t;
				ray_hit.hit = true;
				ray_hit.cell = cell;
				ray_hit.p = p_ray(ray, ray_hit.t_enter);

				ray_hit.color = vec4(f.rgb, 1.0);
			}
			break;
		}

		//find intersection of ray with cell boundary planes
		vec3 pz = -((cell>>level)+side)<<level;
		vec3 tp = -ray.m*(ray.ro+pz);

		//find index of the smallest component of tp that is > t
		tp += vec3(lessThan(tp, vec3(t)))*1e10; //make tp huge when less than t
		ivec3 ix;
		vec3 tp_sort = sort3(tp, ix);

		float min_tp = tp_sort[0];
		int min_ix = ix[0];
		if(ray_hit.t_enter < 0.0)//haven't hit solid yet, normal is the first solid hit
		{
			ray_hit.n = -cell_step*N[min_ix];
		}

		//advance ray to intersection point of next cell
		cell[min_ix] = ((cell[min_ix]>>level)+cell_step[min_ix])<<level;

		t = min_tp;
		
		//If ray exits volume then break out of loop.
		//If we get to the end of the ray then break.
		bool exit_vol = (any(lessThan(cell, min_cell)) || any(greaterThan(cell, max_cell)));
		bool ray_end = (t>=min(ray.tmax, tNF[1]));
		if(exit_vol || ray_end)
		{
			break;
		}
	}
	ray_hit.steps = i;
	return ray_hit;
}

void raycast_add(ivec3 coord)
{
	//if(any(greaterThan(coord, ivec3(ceil(2*brush_r))))) return;
	coord -= ivec3(brush_r);

	Ray ray = Ray(brush_ro, brush_rd, 1.0/brush_rd, 0.0, 1e10);
	RayHit ray_hit = raytrace_grid_3d(ray);

	if(ray_hit.hit==true)
	{
		mRaytraceData.ray_intersection[hand] = vec4(ray_hit.p, 1.0);

		ray_hit.p += 0.99*brush_r*normalize(brush_rd);
		ray_hit.cell = ivec3(floor(ray_hit.p));

		mRaytraceData.dirty_rect_min[hand] = ivec4(ray_hit.cell-ivec3(brush_r), 1);
		mRaytraceData.dirty_rect_max[hand] = ivec4(ray_hit.cell+ivec3(brush_r), 1);

		coord += ray_hit.cell;
		vec4 im = imageLoad(uImage, coord);
		//float d1 = sdSphere(vec3(coord)-vec3(ray_hit.cell), brush_r);
		const float p = BrushUniforms.mPressure[hand];
		float d1 = sdBrush(brush_xform*(vec3(coord)-vec3(ray_hit.cell)), p*BrushUniforms.mSize[hand].xyz);
		float d2 = im.w;

		float d;
	
		if(d1<0.0)
		{
			d = max(-d1, d2);
			//d = d2+0.1*abs(d1);
			//vec3 color = mix(im.rgb, brush_color.rgb, brush_color.a);
			vec3 color = mix(im.rgb, BrushUniforms.mColor[hand].rgb, BrushUniforms.mColor[hand].a);
			imageStore(uImage, coord, vec4(color,d));	
		}
	}
	else //ray missed
	{
		mRaytraceData.ray_intersection[hand] = vec4(-1.0);
		mRaytraceData.dirty_rect_min[hand] = ivec4(-1);
		mRaytraceData.dirty_rect_max[hand] = ivec4(-1);
	}
}

void raycast_sub(ivec3 coord)
{
	//if(any(greaterThan(coord, ivec3(ceil(2*brush_r))))) return;
	coord -= ivec3(brush_r);

	Ray ray = Ray(brush_ro, brush_rd, 1.0/brush_rd, 0.0, 1e10);
	RayHit ray_hit = raytrace_grid_3d(ray);

	ivec3 brush_p = ray_hit.cell+ivec3(0.01*brush_rd);
	if(ray_hit.hit==true)
	{
		mRaytraceData.ray_intersection[hand].xyz = ray_hit.p;

		ray_hit.p -= 1.0*brush_r*normalize(brush_rd);
		ray_hit.cell = ivec3(floor(ray_hit.p));

		mRaytraceData.dirty_rect_min[hand].xyz = ray_hit.cell-ivec3(brush_r);
		mRaytraceData.dirty_rect_max[hand].xyz = ray_hit.cell+ivec3(brush_r);

		//coord += brush_p;
		coord += ray_hit.cell;
		vec4 im = imageLoad(uImage, coord);
		//float d1 = sdSphere(vec3(coord)-vec3(brush_p), brush_r);
		const float p = BrushUniforms.mPressure[hand];
		float d1 = sdBrush(brush_xform*(vec3(coord)-vec3(brush_p)), BrushUniforms.mSize[hand].xyz);
		float d2 = im.w;

		float d;
		if(d1 < 0.0)
		{
			//d = -d1;
			//d = d2-0.1*abs(d1);
			float d = min(d1, d2);
			imageStore(uImage, coord, vec4(im.rgb, d));	
		}
	}
}

void raycast_paint(ivec3 coord)
{
	if(any(greaterThan(coord, ivec3(ceil(2*brush_r))))) return;
	coord -= ivec3(brush_r);

	Ray ray = Ray(brush_ro, brush_rd, 1.0/brush_rd, 0.0, 1e10);
	RayHit ray_hit = raytrace_grid_3d(ray);

	const float p = BrushUniforms.mPressure[hand];
	float d = sdBrush(brush_xform*vec3(coord), p*BrushUniforms.mSize[hand].xyz);

	if(ray_hit.hit==true && d<=0.0)
	{
		coord += ray_hit.cell;
		vec4 im = imageLoad(uImage, coord);
		vec3 color = mix(im.rgb, BrushUniforms.mColor[hand].rgb, p*BrushUniforms.mColor[hand].a);
		imageStore(uImage, coord, vec4(color,im.w));	
	}

}