#version 450

#define TEX_COLOUR              1 << 0
#define TEX_NORMAL              1 << 1
#define TEX_DISPLACE            1 << 2
#define TEX_SPECULAR            1 << 3
#define TEX_AMBIENT_OCCLUSION   1 << 4

#define MAX_LIGHTS 4

layout(triangles, equal_spacing, ccw) in;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
    float alpha;
    vec4 dparams;
    float texScaling;
};

layout (location = 2) uniform sampler2D texDisplace;
layout (location = 5) uniform sampler2D texPerlin;
uniform mat4 Perspective;
uniform mat4 View;
uniform bool enableTessellation;
uniform int texture_flags;
uniform int num_lights;
uniform Material material;

struct Patch
{
    vec3 B030;
    vec3 B021;
    vec3 B012;
    vec3 B003;
    vec3 B102;
    vec3 B201;
    vec3 B300;
    vec3 B210;
    vec3 B120;
    vec3 B111;
    vec3 N[3];
    vec2 uv[3];
};

in patch Patch oPatch;
in vec3 tescReflectVector_WS[];
in vec4 tescShadowCoord[][MAX_LIGHTS];

out vec3 teseNormal_WS;
out vec3 tesePosition_WS;
out vec2 teseTexCoord;
out vec4 teseShadowCoord[MAX_LIGHTS];
out vec3 teseReflectVector_WS;

vec2 lerp2d(vec2 v0, vec2 v1, vec2 v2)
{
   	return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

vec3 lerp3d(vec3 v0, vec3 v1, vec3 v2)
{
   	return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

vec4 lerp4d(vec4 v0, vec4 v1, vec4 v2)
{
   	return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

void main()
{
    teseTexCoord = lerp2d(oPatch.uv[0], oPatch.uv[1], oPatch.uv[2]);
    teseNormal_WS = lerp3d(oPatch.N[0], oPatch.N[1], oPatch.N[2]);
    teseReflectVector_WS = lerp3d(tescReflectVector_WS[0], tescReflectVector_WS[1], tescReflectVector_WS[2]);

    for (int i = 0; i < num_lights; ++i)
    {
        teseShadowCoord[i] = lerp4d(tescShadowCoord[0][i], tescShadowCoord[1][i], tescShadowCoord[2][i]);
    }
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    float uPow3 = pow(u, 3);
    float vPow3 = pow(v, 3);
    float wPow3 = pow(w, 3);
    float uPow2 = pow(u, 2);
    float vPow2 = pow(v, 2);
    float wPow2 = pow(w, 2);

    tesePosition_WS =    oPatch.B300 * wPow3 +
                    oPatch.B030 * uPow3 +
                    oPatch.B003 * vPow3 +
                    oPatch.B210 * 3.0 * wPow2 * u +
                    oPatch.B120 * 3.0 * w * uPow2 +
                    oPatch.B201 * 3.0 * wPow2 * v +
                    oPatch.B021 * 3.0 * uPow2 * v +
                    oPatch.B102 * 3.0 * w * vPow2 +
                    oPatch.B012 * 3.0 * u * vPow2 +
                    oPatch.B111 * 6.0 * w * u * v;

    float displacement = 0;
    float perlinDisplacement = 0;
    if (enableTessellation && (texture_flags & TEX_DISPLACE) != 0 )
    {   
        displacement = material.dparams.x * (texture(texDisplace, material.texScaling * teseTexCoord).x - 0.5);
    
        if (material.dparams.y != 0)
        {
            float perlinValue = (texture(texPerlin, material.dparams.w * teseTexCoord).x) - 0.5;
            perlinDisplacement = material.dparams.y * perlinValue;
        }
    }

    tesePosition_WS += (displacement + perlinDisplacement) * teseNormal_WS;
    gl_Position = Perspective * View * vec4(tesePosition_WS, 1);
}