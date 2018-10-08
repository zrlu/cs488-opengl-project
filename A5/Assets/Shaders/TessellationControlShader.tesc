#version 450

#define TEX_COLOUR              1 << 0
#define TEX_NORMAL              1 << 1
#define TEX_DISPLACE            1 << 2
#define TEX_SPECULAR            1 << 3
#define TEX_AMBIENT_OCCLUSION   1 << 4

#define MAX_LIGHTS 4

layout (vertices = 3) out;

in vec3 vertNormal_WS[];
in vec3 vertReflectVector_WS[];
in vec3 vertPosition_WS[];
in vec2 vertTexCoord[];
in vec4 vertShadowCoord[][MAX_LIGHTS];

uniform mat4 Model;
uniform mat4 View;
uniform vec3 eye;
uniform bool enableTessellation;
uniform int texture_flags;
uniform int num_lights;

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

out patch Patch oPatch;
out vec3 tescReflectVector_WS[];
out vec4 tescShadowCoord[][MAX_LIGHTS];

float getTessLevel(float a, float b)
{
    float d = (a + b) / 2.0;

    return 40 / floor(d);
}

vec3 ProjectToPlane(vec3 point, vec3 planepoint, vec3 unitPlaneNormal)
{
    vec3 v = point - planepoint;
    float Len = dot(v, unitPlaneNormal);
    vec3 d = Len * unitPlaneNormal;
    return (point - d);
}

void CalcPositions()
{
    // The original vertices stay the same
    oPatch.B030 = vertPosition_WS[0];
    oPatch.B003 = vertPosition_WS[1];
    oPatch.B300 = vertPosition_WS[2];

    // Edges are names according to the opposing vertex
    vec3 EdgeB300 = oPatch.B003 - oPatch.B030;
    vec3 EdgeB030 = oPatch.B300 - oPatch.B003;
    vec3 EdgeB003 = oPatch.B030 - oPatch.B300;

    // Generate two midpoints on each edge
    oPatch.B021 = oPatch.B030 + EdgeB300 / 3.0;
    oPatch.B012 = oPatch.B030 + EdgeB300 * 2.0 / 3.0;
    oPatch.B102 = oPatch.B003 + EdgeB030 / 3.0;
    oPatch.B201 = oPatch.B003 + EdgeB030 * 2.0 / 3.0;
    oPatch.B210 = oPatch.B300 + EdgeB003 / 3.0;
    oPatch.B120 = oPatch.B300 + EdgeB003 * 2.0 / 3.0;

    // Project each midpoint on the plane defined by the nearest vertex and its normal
    oPatch.B021 = ProjectToPlane(oPatch.B021, oPatch.B030, oPatch.N[0]);
    oPatch.B012 = ProjectToPlane(oPatch.B012, oPatch.B003, oPatch.N[1]);
    oPatch.B102 = ProjectToPlane(oPatch.B102, oPatch.B003, oPatch.N[1]);
    oPatch.B201 = ProjectToPlane(oPatch.B201, oPatch.B300, oPatch.N[2]);
    oPatch.B210 = ProjectToPlane(oPatch.B210, oPatch.B300, oPatch.N[2]);
    oPatch.B120 = ProjectToPlane(oPatch.B120, oPatch.B030, oPatch.N[0]);

    // Handle the center
    vec3 Center = (oPatch.B003 + oPatch.B030 + oPatch.B300) / 3.0;
    oPatch.B111 = (
        oPatch.B021 + oPatch.B012 + oPatch.B102 +
        oPatch.B201 + oPatch.B210 + oPatch.B120
    ) / 6.0;
    oPatch.B111 += (oPatch.B111 - Center) / 2.0;
}

void main()
{

    // Set data
    for (int i = 0; i < 3; i++)
    {
        oPatch.N[i] = vertNormal_WS[i];
        oPatch.uv[i] = vertTexCoord[i];
        for (int j = 0; j < num_lights; ++j)
        {
            tescShadowCoord[gl_InvocationID][j] = vertShadowCoord[gl_InvocationID][j];
        }

    }

    tescReflectVector_WS[gl_InvocationID] = vertReflectVector_WS[gl_InvocationID];

    CalcPositions();

    float eyeCPdist0 = distance(vertPosition_WS[0], eye);
    float eyeCPdist1 = distance(vertPosition_WS[1], eye);
    float eyeCPdist2 = distance(vertPosition_WS[2], eye);

    if (enableTessellation && (texture_flags & TEX_DISPLACE) != 0)
    {
        gl_TessLevelOuter[0] = getTessLevel(eyeCPdist1, eyeCPdist2);
        gl_TessLevelOuter[1] = getTessLevel(eyeCPdist2, eyeCPdist0);
        gl_TessLevelOuter[2] = getTessLevel(eyeCPdist0, eyeCPdist1);
        gl_TessLevelInner[0] = gl_TessLevelOuter[2];
    } else {
        gl_TessLevelOuter[0] = 1;
        gl_TessLevelOuter[1] = 1;
        gl_TessLevelOuter[2] = 1;
        gl_TessLevelInner[0] = 0;
    }
}
