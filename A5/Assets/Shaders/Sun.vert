#version 450

in vec2 particlePosition;

uniform mat4 Perspective;
uniform mat4 ModelView;

out vec2 uv;

void main(void){

    uv = particlePosition;
	gl_Position = Perspective * ModelView * vec4(particlePosition, 0.0, 1.0);

}