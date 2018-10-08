#version 450

out vec4 FragColor;
in vec2 uv;

uniform float lifeLength;
uniform float elapsedTime;
uniform vec3 particleColor1;
uniform vec3 particleColor2;

float getLifeRatio()
{
	return (lifeLength - elapsedTime) / lifeLength;
}

float getAlpha()
{
	return getLifeRatio() * clamp(getLifeRatio() * getLifeRatio() * 0.5 - length(uv), 0.0, 0.5);
}

void main(void){

	vec3 color = mix(particleColor1, particleColor2, getLifeRatio() * getLifeRatio());
	FragColor = vec4(color, getAlpha());

}
