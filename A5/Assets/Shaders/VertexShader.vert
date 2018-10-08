#version 450

#define TEX_COLOUR              1 << 0
#define TEX_NORMAL              1 << 1
#define TEX_DISPLACE            1 << 2
#define TEX_SPECULAR            1 << 3
#define TEX_AMBIENT_OCCLUSION   1 << 4

#define MAX_LIGHTS 4

// Model-Space coordinates
in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Perspective;
uniform mat3 NormalMatrix;
uniform mat4 depthBiasMVP[MAX_LIGHTS];
uniform int num_lights;
uniform vec3 eye;

out vec3 vertNormal_WS;
out vec3 vertReflectVector_WS;
out vec3 vertPosition_WS;
out vec2 vertTexCoord;
out vec4 vertShadowCoord[MAX_LIGHTS];

void main() {
	vec3 pos = position;
	vertNormal_WS = normalize(NormalMatrix * normal);
	vec4 pos4 = vec4(pos, 1.0);
	vertPosition_WS = (Model * pos4).xyz;
	vertTexCoord = uv;
	for (int i = 0; i < num_lights; ++i)
	{
		vertShadowCoord[i] = depthBiasMVP[i] * pos4;
	}
	vec3 cameraVector = normalize(position - eye);
	vertReflectVector_WS = reflect(cameraVector, vertNormal_WS);
}
