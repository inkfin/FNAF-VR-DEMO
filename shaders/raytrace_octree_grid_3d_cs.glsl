#version 450
#include "intersect_cs.h.glsl"
#line 3

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f, binding = 0) restrict writeonly uniform image2D uImage; 
layout(binding = 1) uniform sampler3D uGrid; 

layout (location=1) uniform mat4 PVinv;

#include "std_uniforms.h.glsl"
#line 14

uniform int min_level = 0;
uniform int max_level = 1;

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

struct RayHit
{
	vec3 p;
	vec3 n;
	ivec3 cell;
	float t_enter;
	float t_exit;
	int steps;
	bool hit;
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

Ray generate_ray_dir(vec2 clip);
RayHit raytrace_grid_3d(in Ray ray, int mode);
float ao_rt(vec3 pos, vec3 n);
float ao_voxel(vec3 pos, vec3 n, ivec3 cell);
vec4 sky_color(vec3 d);

void main()
{
	ivec2 gid = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(uImage);
	ivec3 grid_size = textureSize(uGrid, 0);

	if(any(greaterThanEqual(gid, size))) return;

	//TEMP
	//imageStore(uImage, gid, vec4(1.0, 0.0, 0.0, 1.0));
	//imageStore(uImage, gid, LightUniforms.Ld);
	//return;
	//TEMP

	vec2 clip = 2.0*vec2(gid)/vec2(size)-vec2(1.0);

	Ray ray = generate_ray_dir(clip);
	RayHit ray_hit = raytrace_grid_3d(ray, 0);
	vec4 ksky = sky_color(normalize(ray.rd));
	if(ray_hit.hit==false)
	{
		imageStore(uImage, gid, ksky);
	}
	else
	{
		vec3 l = normalize(LightUniforms.light_w.xyz-ray_hit.p);
		Ray shadow_ray = CreateRay(ray_hit.p+0.1*ray_hit.n, l);
		RayHit shadow_ray_hit = raytrace_grid_3d(shadow_ray, 1);

		//float ss=0.2+0.8*float(!shadow_ray_hit.hit);// hardshadow
		//Fake soft shadow https://www.shadertoy.com/view/WslGz4
		const float sk = 2.0;
		float sh = clamp(0.3*sk*(shadow_ray_hit.t_exit-shadow_ray_hit.t_enter)/shadow_ray_hit.t_exit, 0.0, 1.0);
		float ss = 1.0-sh*sh*(3.0-2.0*sh);
		
		//float ao = ao_rt(ray_hit.p+0.1*ray_hit.n, ray_hit.n);
		float ao = 1.0-0.35*ao_voxel(ray_hit.p, ray_hit.n, ray_hit.cell);
		vec4 I = MaterialUniforms.ka*ao*sky_color(ray_hit.n) + LightUniforms.Ld*MaterialUniforms.kd*ss*max(0.0, dot(ray_hit.n, l));
		vec4 color = I;
		//color = vec4(!shadow_ray_hit.hit);
		//color = vec4(ss);
		//float thickness = ray_hit.t_exit-ray_hit.t_enter;
		//color = 1.1*ksky*vec4(exp(-thickness*140.00*vec4(1.0, 0.01, 0.1, 1.0)));
		//color = ao*sky_color(ray_hit.n);
		//color = vec4(ao);
		//color = vec4(ray_hit.cell/32.0, 1.0);
		//color = vec4(floor(ray_hit.p)/32.0, 1.0);
		//color = vec4(vec3(ray_hit.p.xyz/32.0), 1.0);
		//color = vec4(ray_hit.n, 1.0);
		//color = vec4(l, 1.0);
		//color = Ld*kd*max(0.0, dot(ray_hit.n, l));
		//color = vec4(ray_hit.steps/20.0);

		color = pow(color, vec4(1.0/2.2));

		imageStore(uImage, gid, color);
	}
}

vec4 sky_color(vec3 d)
{
	vec3 l = normalize(-LightUniforms.light_w.xyz);
	float f = dot(d, l);
	return mix(LightUniforms.Ld, LightUniforms.La, 0.5*f+0.5);
}

Ray generate_ray_dir(vec2 clip)
{
	vec4 pfar = vec4(clip, +1.0, 1.0);
	vec4 pnear = vec4(clip, -1.0, 1.0);

	pfar = PVinv*pfar;
	pfar = pfar/pfar.w;
	pnear = PVinv*pnear;
	pnear = pnear/pnear.w;

	vec3 rd = pfar.xyz-pnear.xyz;
	return CreateRay(pnear.xyz, rd, 0.0, 1.0);
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

//mode==0: return on ray enter (solid hit)
//mode==1: return on ray exit solid
//mode==2: return on ray exit box //TODO not implemented

RayHit raytrace_grid_3d(in Ray ray, int mode)
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

	ivec3 cell = ivec3(floor(ray_hit.p));
	cell = clamp(cell, min_cell, max_cell);
	
	int i;
	for(i=0; i<4000; i++)
	{
		vec4 f = texelFetch(uGrid, cell>>level, level); //0: empty space

		//TODO fix mode==1
		if(f.w == 0.0 && level < max_level && mode != 1)
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
			}
			if(mode==0) break;
		}

		//if mode==1 and we exit the solid then break
		if(mode==1 && f.r==0.0 && ray_hit.t_enter > 0.0 && level == min_level)
		{
			ray_hit.t_exit = t;
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
			if(mode==1 && ray_hit.t_enter > 0.0)
			{
				ray_hit.t_exit = t;
			}
			break;
		}
	}
	ray_hit.steps = i;
	return ray_hit;
}


float ao_voxel(vec3 pos, vec3 n, ivec3 cell)
{
	const int level = min_level;
	const ivec3 level_size = textureSize(uGrid, level);
	const int scale = 1<<level;

	ivec3 ni = ivec3(sign(n));
	ivec3 ti = ni.yzx;
	ivec3 bi = ni.zxy;

	vec3 box_hw = scale*(vec3(0.5)+0.01*(abs(ti)+abs(bi))); //the size of boxes we compute ao for

	float ao = 0.0;

	const int r = 3; //voxel search radius
	for(int i=-r; i<=+r; i++)
	for(int j=-r; j<=+r; j++)
	for(int k=1; k<=r; k++)
	{
		ivec3 cell_ijk = cell/scale + i*ti + j*bi + k*ni;
		if(any(greaterThanEqual(cell_ijk, level_size))) continue;
		if(any(lessThan(cell_ijk, ivec3(0)))) continue;
		vec4 f = texelFetch(uGrid, cell_ijk, level);
		if(f.w > 0.0)
		{
			vec3 box_pos = scale*(cell_ijk + vec3(0.5)) + 0.05*n;
			float ao_ijk = boxOcclusionOpt(pos, n, box_pos, box_hw);
			ao += ao_ijk;
		}	
	}
	return ao;
}

vec2 hash2( float n ) { return fract(sin(vec2(n,n+1.0))*vec2(43758.5453123,22578.1459123)); }

float ao_rt(vec3 pos, vec3 n)
{
	Ray ao_ray;
	ao_ray.ro = pos;
	ao_ray.tmin = 0.0;
	ao_ray.tmax = 2.0;

	float vis = 0.0;
	const int samples = 26;

	vec3 ru = n.yzx;
	vec3 rv = n.zxy;

	for(int i=0; i<samples; i++)
	{
		// cosine distribution
        vec2  aa = hash2( pos.x + pos.y + pos.z + float(i)*203.111 );
        float ra = sqrt(aa.y);
        float rx = ra*cos(6.2831*aa.x); 
        float ry = ra*sin(6.2831*aa.x);
        float rz = sqrt( 1.0-aa.y );
        vec3 dir = vec3( rx*ru + ry*rv + rz*n);
		ao_ray.rd = dir;
		ao_ray.m = 1.0/ao_ray.rd;

		RayHit ray_hit = raytrace_grid_3d(ao_ray, 1);
		vis += float(ray_hit.hit==false);
	}
	return vis/float(samples);
}