#version 450

in vec2 TextureCoords;

out vec4 FragColor;

layout (location = 0) uniform sampler2D flareTexture;
uniform float brightness;

void main(void)
{
    FragColor = texture(flareTexture, TextureCoords);
    FragColor.a *= brightness;
}