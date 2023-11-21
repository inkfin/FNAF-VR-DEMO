#version 430
layout(binding = 0) uniform sampler2D color_tex; 
layout(location = 2) uniform float time;
layout(location = 4) uniform int Mode;

vec4 eye_w = vec4(0.0f, 1.5f, 0.5f, 1.0f);	//world-space eye position

//vec4 La = vec4(1.0);	//ambient light color
//vec4 Ld = vec4(1.0);	//diffuse light color
//vec4 Ls = vec4(1.0);	//specular light color
//vec4 light_w = vec4(0.0, 0.0, 1.0, 0.0); //world-space light position

//vec4 ka = vec4(1.0);	//ambient material color
//vec4 kd = vec4(1.0);	//diffuse material color
//vec4 ks = vec4(1.0);	//specular material color
//float shininess = 10.0; //specular exponent


layout(std140, binding = 1) uniform LightUniforms
{
   vec4 La;	//ambient light color
   vec4 Ld;	//diffuse light color
   vec4 Ls;	//specular light color
   vec4 light_w; //world-space light position
};

layout(std140, binding = 2) uniform MaterialUniforms
{
   vec4 ka;	//ambient material color
   vec4 kd;	//diffuse material color
   vec4 ks;	//specular material color
   float shininess; //specular exponent
};

in VertexData
{
   vec2 tex_coord;
   vec3 pw;       //world-space vertex position
   vec3 nw;   //world-space normal vector
   float w_debug;
} inData;   //block is named 'inData'

out vec4 fragcolor; //the output color for this fragment    

void main(void)
{   
   //Compute per-fragment Phong lighting
   vec4 ktex = texture(color_tex, inData.tex_coord);

   if(Mode==2)
   {
      const vec4 debug_color = vec4(1.0, 0.0, 0.35, 1.0);
      ktex = mix(ktex, debug_color, inData.w_debug);
   }
	
   vec4 ambient_term = ka*ktex*La;

   const float eps = 1e-8; //small value to avoid division by 0
   float d = distance(light_w.xyz, inData.pw.xyz);
   //float atten = 1.0/(d*d+eps); 
   float atten = 1.0; //ignore attenuation

   vec3 nw = normalize(inData.nw);			//world-space unit normal vector
   vec3 lw = normalize(light_w.xyz - inData.pw.xyz);	//world-space unit light vector
   vec4 diffuse_term = atten*kd*ktex*Ld*max(0.0, dot(nw, lw));

   vec3 vw = normalize(eye_w.xyz - inData.pw.xyz);	//world-space unit view vector
   vec3 rw = reflect(-lw, nw);	//world-space unit reflection vector

   vec4 specular_term = atten*ks*Ls*pow(max(0.0, dot(rw, vw)), shininess);

   fragcolor = ambient_term + diffuse_term + specular_term;
}
