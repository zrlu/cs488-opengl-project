#include "CubeMapTexture.hpp"
#include "loadBMP.hpp"
#include <iostream>
#include "cs488-framework/GlErrorCheck.hpp"
#include <lodepng/lodepng.h>

using namespace std;

CubemapTexture::CubemapTexture()
{}

CubemapTexture::CubemapTexture(
        const string& Directory,
        const string& PosXFilename,
        const string& NegXFilename,
        const string& PosYFilename,
        const string& NegYFilename,
        const string& PosZFilename,
        const string& NegZFilename
)
{
    m_fileNames[0] = Directory + "/" + PosXFilename;
    m_fileNames[1] = Directory + "/" + NegXFilename;
    m_fileNames[2] = Directory + "/" + PosYFilename;
    m_fileNames[3] = Directory + "/" + NegYFilename;
    m_fileNames[4] = Directory + "/" + PosZFilename;
    m_fileNames[5] = Directory + "/" + NegZFilename;

}

bool CubemapTexture::Load(void (*mipmapfunc)(GLenum))
{
    glGenTextures(1, &m_textureObj);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);

    unsigned char *bitmap;
    unsigned int width, height;

    for (unsigned int i = 0; i < 6; i++)
    {
	    lodepng_decode_file(&bitmap, &width, &height, m_fileNames[i].c_str(), LodePNGColorType::LCT_RGBA, 8);

        cerr << "Cubemap: " << m_fileNames[i] << endl;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height , 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
       
   
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
   
        delete [] bitmap;
    }
    mipmapfunc(GL_TEXTURE_CUBE_MAP);
    CHECK_GL_ERRORS;
    return true;
}

void CubemapTexture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
}

CubemapTexture::~CubemapTexture() {}