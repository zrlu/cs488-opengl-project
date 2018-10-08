#version 450

in vec3 position;                                             
                                                                                    
uniform mat4 Perspective;                                                                  
uniform mat4 View;

out vec3 TexCoord;                                                                 
                                                                                    
void main()                                                                         
{                                                                                   
    gl_Position = Perspective * mat4(View[0], View[1], View[2], vec4(0, 0, 0, 1)) * vec4(position.x, position.y, position.z, 1.0);                                                                                        
    TexCoord = position;                                                         
}
