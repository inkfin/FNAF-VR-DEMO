#version 450
#include "std_uniforms.h.glsl"
#include "intersect_cs.h.glsl"
#include "picking.h.glsl"
#line 5

in VertexData
{
   vec2 tex_coord;
   vec2 clip;
} inData; 

out vec4 fragcolor;

layout(binding = 1) uniform sampler3D uGrid; 

layout (location=1) uniform mat4 M;
layout (location=2) uniform mat4 Minv;
layout (location=3) uniform mat4 PVinv;


uniform int min_level = 0;
uniform int max_level = 9;

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
	int steps;
	bool hit;
	vec4 color;
};

RayHit CreateRayHit()
{
	RayHit r;
	r.hit = false;
	r.t_enter = -1;
	r.steps = 0;
	return r;
}

Ray generate_ray_dir(vec2 clip);
RayHit raytrace_grid_3d(in Ray ray);
float ao_rt(vec3 pos, vec3 n);
float ao_voxel(vec3 pos, vec3 n, ivec3 cell);
float fast_ao_voxel(vec3 pos, vec3 n, ivec3 cell, int level);
vec4 sky_color(vec3 d);

void main()
{
	ivec2 gid = ivec2(gl_FragCoord.xy);
	//const ivec3 grid_size = textureSize(uGrid, 0);

	Ray ray = generate_ray_dir(inData.clip);
	RayHit ray_hit = raytrace_grid_3d(ray);
	vec4 ksky = sky_color(normalize(ray.rd));

	if(ray_hit.hit==false)
	{
		fragcolor = ksky;
		gl_FragDepth = gl_DepthRange.far-1e-6;
	}
	else
	{
		vec3 l = normalize(Minv*vec4(LightUniforms.light_w.xyz-ray_hit.p, 1.0)).xyz;
		//Ray shadow_ray = CreateRay(ray_hit.p+0.1*ray_hit.n, l);
		//RayHit shadow_ray_hit = raytrace_grid_3d(shadow_ray);
		//float ss=0.2+0.8*float(!shadow_ray_hit.hit);// hardshadow

		float ss = 0.0;
		for(int i=0; i<3; i++)
		{
			Ray shadow_ray = CreateRay(ray_hit.p+0.1*ray_hit.n, l);
			RayHit shadow_ray_hit = raytrace_grid_3d(shadow_ray);
			l.xyz = l.yzx;
			ss += 0.33*(0.2+0.8*float(!shadow_ray_hit.hit));// hardshadow
		}

		//float ao = ao_rt(ray_hit.p+0.1*ray_hit.n, ray_hit.n);
		//float ao = 1.0-0.3*ao_voxel(ray_hit.p, ray_hit.n, ray_hit.cell);

		//float ao0 = fast_ao_voxel(ray_hit.p, ray_hit.n, ray_hit.cell, min_level);
		//float ao1 = fast_ao_voxel(ray_hit.p, ray_hit.n, ray_hit.cell, min_level+1);
		//float ao2 = fast_ao_voxel(ray_hit.p, ray_hit.n, ray_hit.cell, min_level+2);
		//float ao3 = fast_ao_voxel(ray_hit.p, ray_hit.n, ray_hit.cell, min_level+3);
		//float ao = min(min(ao0, ao1),min(ao2, ao3));


		float ao = fast_ao_voxel(ray_hit.p, ray_hit.n, ray_hit.cell, min_level);

		//vec4 I = MaterialUniforms.ka*ao*sky_color(ray_hit.n) + LightUniforms.Ld*MaterialUniforms.kd*ss*max(0.0, dot(ray_hit.n, l)+0.2);
		vec4 I = ray_hit.color*ao*sky_color(ray_hit.n) + (ao+0.5)*LightUniforms.Ld*ray_hit.color*ss*max(0.0, dot(ray_hit.n, l)+0.2);
		vec4 color = I;
		//color = vec4(!shadow_ray_hit.hit);
		//color = vec4(ss);
		//float thickness = ray_hit.t_exit-ray_hit.t_enter;
		//color = 1.1*ksky*vec4(exp(-thickness*140.00*vec4(1.0, 0.01, 0.1, 1.0)));
		//color = ao*sky_color(ray_hit.n);
		//color = vec4(ao);
		//color = vec4(ray_hit.cell/64.0, 1.0);
		//color = vec4(floor(ray_hit.p)/1000.0, 1.0);
		//color = vec4(vec3(ray_hit.p.xyz/1024.0), 1.0);
		//color = vec4(ray_hit.n, 1.0);
		//color = vec4(l, 1.0);
		//color = Ld*kd*max(0.0, dot(ray_hit.n, l));
		//color = vec4(ray_hit.steps/20.0);
		//color = MaterialUniforms.kd;

		color = pow(color, vec4(1.0/1.2));
		fragcolor = color;


		//Show mouseover
			
			vec3 p_left, p_right;
			ivec2 id_left = GetLeftPick(p_left);
			ivec2 id_right = GetRightPick(p_right);
			if(id_left[0]==3 && distance(ray_hit.p, (Minv*vec4(p_left,1.0)).xyz)<16.0)
			{
				fragcolor *= 0.75;			
			}
			if(id_right[0]==3 && distance(ray_hit.p, (Minv*vec4(p_right,1.0)).xyz)<16.0)
			{
				fragcolor *= 0.75;
			}

		//Picking
			vec4 pw = M*vec4(ray_hit.p, 1.0);
			PickPassOutput(pw.xyz, 0);

		//Compute depth
		vec4 eye_space_pos = SceneUniforms.V*pw;
		vec4 clip_space_pos = SceneUniforms.P*eye_space_pos;
		float ndc_depth = clip_space_pos.z / clip_space_pos.w;
		gl_FragDepth = 0.5*(gl_DepthRange.diff*clip_space_pos.z/clip_space_pos.w + gl_DepthRange.near+gl_DepthRange.far);
	}
}

vec4 sky_color(vec3 d)
{
	vec3 l = normalize(LightUniforms.light_w.xyz);
	float f = dot(d, l);
	return mix(LightUniforms.La, LightUniforms.Ld, max(0.0, f));
}

Ray generate_ray_dir(vec2 clip)
{
	vec4 pfar = vec4(clip, +1.0, 1.0);
	vec4 pnear = vec4(clip, -1.0, 1.0);

	pfar = Minv*PVinv*pfar;
	pfar = pfar/pfar.w;
	pnear = Minv*PVinv*pnear;
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
	for(i=0; i<400; i++)
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
				ray_hit.p = p_ray(ray, ray_hit.t_enter);
				ray_hit.cell = cell;//ivec3(floor(ray_hit.p));
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

float getVoxel(ivec3 cell, int level)
{
	return float(texelFetch(uGrid, cell, level).w > 0.0);
}

float vertexAo(vec2 side, float corner) {
	return (side.x + side.y + max(corner, side.x * side.y)) / 3.0;
}

vec4 voxelAo(ivec3 pos, ivec3 d1, ivec3 d2, int level) {
	vec4 side = vec4(getVoxel(pos + d1, level), getVoxel(pos + d2, level), getVoxel(pos - d1, level), getVoxel(pos - d2, level));
	vec4 corner = vec4(getVoxel(pos + d1 + d2, level), getVoxel(pos - d1 + d2, level), getVoxel(pos - d1 - d2, level), getVoxel(pos + d1 - d2, level));
	vec4 ao;
	ao.x = vertexAo(side.xy, corner.x);
	ao.y = vertexAo(side.yz, corner.y);
	ao.z = vertexAo(side.zw, corner.z);
	ao.w = vertexAo(side.wx, corner.w);
	return vec4(1.1) - ao;
}

float fast_ao_voxel(vec3 pos, vec3 n, ivec3 cell, int level)
{
	const int scale = 1<<level;
	ivec3 ni = ivec3(sign(n));
	ivec3 ti = abs(ni.yzx);
	ivec3 bi = abs(ni.zxy);

	vec4 vao = voxelAo(cell/scale+ni, ti, bi, level);

	vec3 uvw = (pos/scale-vec3(cell/scale));
	vec2 uv = abs(vec2(dot(uvw, ti), dot(uvw, bi)));

	float interpAo = mix(mix(vao.z, vao.w, uv.x), mix(vao.y, vao.x, uv.x), uv.y);
	interpAo = pow(interpAo, 0.75);

	//interpAo = dot(vao, vec4(0.25)); //debug
	//interpAo = uv.y; //debug
	return interpAo;
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

		RayHit ray_hit = raytrace_grid_3d(ao_ray);
		vis += float(ray_hit.hit==false);
	}
	return vis/float(samples);
}