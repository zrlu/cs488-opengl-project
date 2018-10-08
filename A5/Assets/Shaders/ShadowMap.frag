#version 450

layout(location = 0) out float fragmentdepth;
// out vec4 fragColor;
void main()
{
    // fragColor = vec4(gl_FragCoord.z * vec3(1, 1, 1), 1);
    fragmentdepth =  gl_FragCoord.z;
}