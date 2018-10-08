#version 450

in vec2 position;

uniform mat4 Perspective;
uniform mat4 ModelView;

out vec2 uv;

void main(void){

    uv = position;
	gl_Position = Perspective * ModelView * vec4(position, 0.0, 1.0);

}