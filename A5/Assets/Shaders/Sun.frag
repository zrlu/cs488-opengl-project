#version 450

out vec4 FragColor;
in vec2 uv;

uniform vec3 sunPosition;
uniform vec3 cameraVector;

float getAlpha()
{
	float r = length(uv);
	float factor = floor(uv.x * 10 + uv.y * 10);
	float width = 0.495;
	float threshold = 0.5 - width;
	if (r < threshold)
	{
		return 1.0;
	}
	if (r >= threshold)
	{
		float k = clamp(dot(normalize(sunPosition), -normalize(cameraVector)), 0.0, 1.0);
		return clamp( (1.0 - pow((r - threshold),  k) / width) - (1 - k), 0.0, 1.0);
	}
	return 0.0;
}

void main(void){
	FragColor = vec4(vec3(1, 1, 1), getAlpha());

}
