#pragma once

#include <string>
#include <GL/gl.h>

using namespace std;

class CubemapTexture
{
public:
    CubemapTexture();
    CubemapTexture(
        const string& Directory,
        const string& PosXFilename,
        const string& NegXFilename,
        const string& PosYFilename,
        const string& NegYFilename,
        const string& PosZFilename,
        const string& NegZFilename
    );

    ~CubemapTexture();

    bool Load(void (*mipmapfunc)(GLenum));

    void Bind(GLenum TextureUnit);

    GLuint textureId[6];

private:

    string m_fileNames[6];
    GLuint m_textureObj;
};