#version 450

in vec2 position;

out vec2 TextureCoords;

uniform vec4 transform;

void main(void){
	
	//calc texture coords based on position
	TextureCoords = position + vec2(0.5, 0.5);
	//apply position and scale to quad
	vec2 screenPosition = position * transform.zw + transform.xy;
	
	//convert to OpenGL coordinate system (with (0,0) in center of screen)
	screenPosition.x = screenPosition.x * 2.0 - 1.0;
	screenPosition.y = screenPosition.y * -2.0 + 1.0;
	gl_Position = vec4(screenPosition, 0.0, 1.0);

}