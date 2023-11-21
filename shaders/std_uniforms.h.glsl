
const int kSceneUniformsBinding = 61;
const int kLightUniformsBinding = 62;
const int kMaterialUniformsBinding = 63;

layout(std140, binding = kSceneUniformsBinding) uniform SceneData
{
      //Scene Uniforms
      vec4 clear_color;
      vec4 fog_color;

      //Camera uniforms
      mat4 P;
      mat4 V;
      mat4 PV;	   //camera projection * view matrix
      mat4 P_ortho;
      vec4 eye_w;	//world-space eye position
      ivec4 Viewport;
      float ViewportAspect;
      float pad0;
      float pad1;
      float pad2;

      //Frame Uniforms
      ivec4 MouseButtonState;
      vec4 LmbClickAndDrag; //xy=drag pos, zw = click pos
      vec4 MmbClickAndDrag; //xy=drag pos, zw = click pos
      vec4 RmbClickAndDrag; //xy=drag pos, zw = click pos
      vec4 MousePos; //xy = mouse pos, zw = delta
      vec4 ScrollPos; //xy = scroll pos, zw = offset
      float Time;
      float DeltaTime;
      int Frame;
      float pad5;

      //Object Uniforms
      mat4 M;
} SceneUniforms;

layout(std140, binding = kLightUniformsBinding) uniform LightData
{
   vec4 La;	//ambient light color
   vec4 Ld;	//diffuse light color
   vec4 Ls;	//specular light color
   vec4 light_w; //world-space light position
   vec4 quad_atten;
} LightUniforms;

layout(std140, binding = kMaterialUniformsBinding) uniform MaterialData
{
   vec4 ka;	//ambient material color
   vec4 kd;	//diffuse material color
   vec4 ks;	//specular material color
   float shininess; //specular exponent
} MaterialUniforms;