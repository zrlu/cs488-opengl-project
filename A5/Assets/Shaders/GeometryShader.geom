#version 450

#define TEX_COLOUR              1 << 0
#define TEX_NORMAL              1 << 1
#define TEX_DISPLACE            1 << 2
#define TEX_SPECULAR            1 << 3
#define TEX_AMBIENT_OCCLUSION   1 << 4

#define MAX_LIGHTS 4

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int num_lights;

in vec3 teseNormal_WS[3];
in vec3 tesePosition_WS[3];
in vec2 teseTexCoord[3];
in vec3 teseReflectVector_WS[3];
in vec4 teseShadowCoord[3][MAX_LIGHTS];

out vec3 geomNormal_WS;
out vec3 geomPosition_WS;
out vec2 geomTexCoord;
out vec3 geomTangent_WS;
out vec4 geomShadowCoord[MAX_LIGHTS];
out vec3 geomReflectVector_WS;

vec3 calculateTangent(vec3 pos[3], vec2 uv[3])
{
    vec3 dXYZdU = pos[1] - pos[0];
    float dSdU = uv[1].s - uv[0].s;
    vec3 dXYZdV = pos[2] - pos[0];
    float dSdV = uv[2].s - uv[0].s;
    vec3 tangent = dSdV * dXYZdU - dSdU * dXYZdV;
    float signedArea = determinant(
        mat2(
            vec2(uv[1].s - uv[0].s, uv[2].s - uv[0].s),
            vec2(uv[1].t - uv[0].t, uv[2].t - uv[0].t)
        )
    );
    if (signedArea < 0)
    {
        tangent = -tangent;
    }
    return tangent;
}

void main()
{
    int i;
    for(i = 0; i < 3; i++)
    {
        geomNormal_WS = teseNormal_WS[i];
        geomPosition_WS = tesePosition_WS[i];
        geomTexCoord = teseTexCoord[i];
        geomTangent_WS = calculateTangent(tesePosition_WS, teseTexCoord);
        geomReflectVector_WS = teseReflectVector_WS[i];
        gl_Position = gl_in[i].gl_Position;
        for (int j = 0; j < num_lights; ++j)
        {
            geomShadowCoord[j] = teseShadowCoord[i][j];
        }
        EmitVertex();
	}
	EndPrimitive();
}
