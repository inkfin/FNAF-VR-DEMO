#version 450 

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 10) out;

in VertexData
{
	flat int cull;
	flat float hw;
	float v;
} inData[];

out GeomData
{
	vec2 uv;
} outData;

void bevel_lines();
void no_join_lines();

void main()
{
	for(int i=0; i<4; i++)
	{
		if(inData[i].cull != 0) return;
		if(gl_in[i].gl_Position.w == 0.0) return;
	}

	const float eps = 1e-8;
	if(distance(gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz)<eps)
	{
		return;
	}

	if(distance(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz)<eps || 
	   distance(gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz)<eps)
	{
		no_join_lines();
	}
	else
	{
		bevel_lines();
	}
}


vec4 safe_normalize(vec4 v, float eps)
{
	return v/(length(v)+eps);
}

void bevel_lines()
{
	float hw = inData[0].hw;

	vec4 p0 = gl_in[0].gl_Position/gl_in[0].gl_Position.w;
	vec4 p1 = gl_in[1].gl_Position/gl_in[1].gl_Position.w;
	vec4 p2 = gl_in[2].gl_Position/gl_in[2].gl_Position.w;
	vec4 p3 = gl_in[3].gl_Position/gl_in[3].gl_Position.w;

	const float eps = 1e-8;

	vec4 dir0 = safe_normalize(p1-p0, eps);
	vec4 dir1 = safe_normalize(p2-p1, eps);
	vec4 dir2 = safe_normalize(p3-p2, eps);

	vec4 n0 = vec4(-dir0.y, dir0.x, dir0.zw);
	vec4 n1 = vec4(-dir1.y, dir1.x, dir1.zw);
	vec4 n2 = vec4(-dir2.y, dir2.x, dir2.zw);

	float left0 = dot(n0, dir1);
	float left1 = dot(n1, dir2);

	vec4 t0 = safe_normalize(dir0+dir1, eps);
	vec4 t1 = safe_normalize(dir1+dir2, eps);

	vec4 m0 = vec4(-t0.y, t0.x, t0.zw);
	vec4 m1 = vec4(-t1.y, t1.x, t1.zw);

	float d0 = hw/abs(dot(m0.xy, n0.xy)+eps);
	float d1 = hw/abs(dot(m1.xy, n1.xy)+eps);

	d0 = min(d0, 2.0*hw);
	d1 = min(d1, 2.0*hw);

	//start half-bevel
	if(left0<0.0)
	{
		gl_Position = p1-d0*m0; //miter
		outData.uv = vec2(0.0, inData[1].v);
		EmitVertex();
		gl_Position = p1+hw*n1; //no join
		outData.uv = vec2(1.0, inData[1].v);
		EmitVertex();
		gl_Position = mix(p1+hw*n1, p1+hw*n0, 0.5); //mid join
		outData.uv = vec2(1.0, inData[1].v);
		EmitVertex();
	}
	else
	{
		gl_Position = p1+d0*m0; //miter
		outData.uv = vec2(1.0, inData[1].v);
		EmitVertex();
		gl_Position = p1-hw*n1; //no join
		outData.uv = vec2(0.0, inData[1].v);
		EmitVertex();
		gl_Position = mix(p1-hw*n1, p1-hw*n0, 0.5); //mid join
		outData.uv = vec2(0.0, inData[1].v);
		EmitVertex();
	}
	EndPrimitive();

	//main segment
	if(left0<0.0)
	{
		gl_Position = p1-d0*m0; //miter
		outData.uv = vec2(0.0, inData[1].v);
		EmitVertex();
		gl_Position = p1+hw*n1; //no join
		outData.uv = vec2(1.0, inData[1].v);
		EmitVertex();
	}
	else
	{
		gl_Position = p1-hw*n1; //no join
		outData.uv = vec2(0.0, inData[1].v);
		EmitVertex();
		gl_Position = p1+d0*m0; //miter
		outData.uv = vec2(1.0, inData[1].v);
		EmitVertex();
	}
	
	if(left1<0.0)
	{
		gl_Position = p2-d1*m1; //miter
		outData.uv = vec2(0.0, inData[2].v);
		EmitVertex();
		gl_Position = p2+hw*n1;	//no join
		outData.uv = vec2(1.0, inData[2].v);
		EmitVertex();
	}
	else
	{
		gl_Position = p2-hw*n1;	//no join
		outData.uv = vec2(0.0, inData[2].v);
		EmitVertex();
		gl_Position = p2+d1*m1;	//miter
		outData.uv = vec2(1.0, inData[2].v);
		EmitVertex();
	}
	
	EndPrimitive();

	//end half-bevel
	if(left1<0.0)
	{
		gl_Position = p2-d1*m1; //miter
		outData.uv = vec2(0.0, inData[2].v);
		EmitVertex();
		gl_Position = p2+hw*n1;	//no join
		outData.uv = vec2(1.0, inData[2].v);
		EmitVertex();
		gl_Position = mix(p2+hw*n1, p2+hw*n2, 0.5); //mid join
		outData.uv = vec2(1.0, inData[2].v);
		EmitVertex();
	}
	else
	{
		gl_Position = p2+d1*m1;	//miter
		outData.uv = vec2(1.0, inData[2].v);
		EmitVertex();
		gl_Position = p2-hw*n1;	//no join
		outData.uv = vec2(0.0, inData[2].v);
		EmitVertex();
		gl_Position = mix(p2-hw*n1, p2-hw*n2, 0.5); //mid join
		outData.uv = vec2(0.0, inData[2].v);
		EmitVertex();
	}

	EndPrimitive();
}

void no_join_lines()
{
	float hw = inData[0].hw;
	vec4 p1 = gl_in[1].gl_Position/gl_in[1].gl_Position.w;
	vec4 p2 = gl_in[2].gl_Position/gl_in[2].gl_Position.w;

	vec4 dir1 = normalize(p2-p1);
	vec4 n1 = vec4(-dir1.y, dir1.x, dir1.zw);

	gl_Position = p1-hw*n1;
	outData.uv = vec2(0.0, inData[1].v);
	EmitVertex();

	gl_Position = p1+hw*n1;
	outData.uv = vec2(1.0, inData[1].v);
	EmitVertex();

	gl_Position = p2-hw*n1;
	outData.uv = vec2(0.0, inData[2].v);
	EmitVertex();

	gl_Position = p2+hw*n1;
	outData.uv = vec2(1.0, inData[2].v);
	EmitVertex();
	
	EndPrimitive();
}
