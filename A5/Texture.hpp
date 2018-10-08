#pragma once

#include <GL/gl.h>

#define TEX_COLOUR              1 << 0
#define TEX_NORMAL              1 << 1
#define TEX_DISPLACE            1 << 2
#define TEX_SPECULAR            1 << 3
#define TEX_AMBIENT_OCCLUSION   1 << 4

typedef int texflags_t;

class Texture
{
public:
    unsigned int width;
    unsigned int height;
    texflags_t flags;
    GLuint colourTextureID;
    GLuint normalTextureID;
    GLuint displaceTextureID;
    GLuint specularTextureID;
    GLuint ambientOcclusionTextureID;
};