#version 450                                                            
                                                                                    
in vec3 TexCoord;                                                                  
                                                                                    
out vec4 FragColor;                                                                 
                                                                                    
uniform samplerCube cubemapTexture;                                                
uniform samplerCube cubemapTextureNight;
uniform float blendFactor;                                                

vec4 getBlendedColor(samplerCube c1, samplerCube c2, vec3 tc, float bf)
{
    vec4 texture1 = texture(c1, tc);
    vec4 texture2 = texture(c2, tc);
    return mix(texture1, texture2, bf);
}

void main()                                                                         
{    
    FragColor = getBlendedColor(cubemapTexture, cubemapTextureNight, TexCoord, blendFactor);                
}
