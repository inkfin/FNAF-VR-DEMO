#version 450
layout(binding = 0) uniform sampler2D color_tex;
layout(location = 2) uniform float time;
layout(location = 4) uniform int Mode;
layout(location = 6) uniform vec4 eye_w;
layout(location = 7) uniform float shininess;

#define POINT_LIGHT_COUNT 4
struct PointLight {
    vec3 position;
    vec3 La;//ambient light color
    vec3 Ld;//diffuse light color
    vec3 Ls;//specular light color

    float constant;
    float linear;
    float quadratic;

    bool isOn;
};
uniform PointLight pointLights[POINT_LIGHT_COUNT];

struct DirLight {
    vec3 direction;

    vec3 La;//ambient light color
    vec3 Ld;//diffuse light color
    vec3 Ls;//specular light color;

    bool is_on;
};
uniform DirLight dirLight;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;

    vec3 La;//ambient light color
    vec3 Ld;//diffuse light color
    vec3 Ls;//specular light color;

    float constant;
    float linear;
    float quadratic;

    bool isOn;
};
uniform SpotLight spotLight;

in VertexData
{
    vec2 tex_coord;
    vec3 pw;//world-space vertex position
    vec3 nw;//world-space normal vector
    float w_debug;
} inData;//block is named 'inData'

out vec4 fragcolor;//the output color for this fragment


vec3 directional_shading(DirLight light, vec3 ktex) {
    vec3 lw = normalize(-light.direction);//world-space unit light vector
    vec3 nw = normalize(inData.nw);//world-space unit normal vector
    vec3 vw = normalize(eye_w.xyz - inData.pw.xyz);//world-space unit view vector

    vec3 ambient_term = ktex * light.La;

    vec3 diffuse_term = ktex * light.Ld * max(0.0, dot(nw, lw));

    vec3 rw = reflect(-lw, nw);//world-space unit reflection vector
    vec3 specular_term = light.Ls * pow(max(0.0, dot(rw, vw)), shininess);

    return ambient_term + diffuse_term + specular_term;
}

vec3 spotlight_shading(SpotLight light, vec3 ktex) {
    vec3 lw = normalize(light.position.xyz - inData.pw.xyz);//world-space unit light vector
    float spot_factor = dot(-lw, normalize(light.direction));
    if (spot_factor < light.cutoff) {
        return vec3(0.0);
    }

    vec3 nw = normalize(inData.nw);//world-space unit normal vector
    vec3 vw = normalize(eye_w.xyz - inData.pw.xyz);//world-space unit view vector

    vec3 ambient_term = ktex * light.La;

    const float eps = 1e-8;// small value to avoid division by 0
    float d = distance(light.position.xyz, inData.pw.xyz);
    float atten = 1.0 / (light.constant + light.linear * d + light.quadratic * d * d + eps);
    // float atten = 1.0;//ignore attenuation

    vec3 diffuse_term = atten * ktex * light.Ld * max(0.0, dot(nw, lw));

    vec3 rw = reflect(-lw, nw);//world-space unit reflection vector

    vec3 specular_term = atten * light.Ls * pow(max(0.0, dot(rw, vw)), shininess);

    vec3 color = ambient_term + diffuse_term + specular_term;
    return color * (1.0 - (1.0 - spot_factor) / (1.0 - light.cutoff));
}

vec3 phone_shading(PointLight light, vec3 ktex)
{
    vec3 lw = normalize(light.position.xyz - inData.pw.xyz);//world-space unit light vector
    vec3 nw = normalize(inData.nw);//world-space unit normal vector
    vec3 vw = normalize(eye_w.xyz - inData.pw.xyz);//world-space unit view vector

    vec3 ambient_term = ktex * light.La;

    const float eps = 1e-8;// small value to avoid division by 0
    float d = distance(light.position.xyz, inData.pw.xyz);
    float atten = 1.0 / (light.constant + light.linear * d + light.quadratic * d * d + eps);
    // float atten = 1.0;//ignore attenuation

    vec3 diffuse_term = atten * ktex * light.Ld * max(0.0, dot(nw, lw));

    vec3 rw = reflect(-lw, nw);//world-space unit reflection vector

    vec3 specular_term = atten * light.Ls * pow(max(0.0, dot(rw, vw)), shininess);

    return ambient_term + diffuse_term + specular_term;
}

void main(void)
{
    //Compute per-fragment Phong lighting
    vec4 ktex = texture(color_tex, inData.tex_coord);

    if (Mode==2)
    {
        const vec4 debug_color = vec4(1.0, 0.0, 0.35, 1.0);
        ktex = mix(ktex, debug_color, inData.w_debug);
    }

    vec3 outColor = vec3(0.0, 0.0, 0.0);
    if (spotLight.isOn) {
        outColor = spotlight_shading(spotLight, ktex.xyz);
    }

    for (int i = 0; i < POINT_LIGHT_COUNT; i++)
    {
        if (!pointLights[i].isOn) continue;
        outColor += phone_shading(pointLights[i], ktex.xyz);
    }

    fragcolor = vec4(outColor, 1.0);
}

