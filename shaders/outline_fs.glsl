#version 450

in VertexData
{
   vec3 p;
} inData; 

out vec4 fragcolor;

float sdBoxFrame( vec3 p, vec3 b, float e )
{
       p = abs(p  )-b;
  vec3 q = abs(p+e)-e;
  return min(min(
      length(max(vec3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
      length(max(vec3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
      length(max(vec3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}

void main(void)
{  
	const float w = 0.003;
	float d = sdBoxFrame(inData.p, vec3(1.0), w);
	if(d>0.001) discard;

	fragcolor = vec4(0.0, 0.0, 0.0, 0.125);
	//fragcolor = vec4(inData.tex_coord, 0.5);
}













