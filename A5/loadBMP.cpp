#include "loadBMP.hpp"

#include <iostream>

int loadBMP(const char *imagepath, unsigned char *&bufferptr, unsigned int &width, unsigned int &height)
{
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;

    FILE * file = fopen(imagepath, "rb");
    if (!file)
    {
        return -1;
    }

    if ( fread(header, 1, 54, file) != 54 )
    {
        return -1;
    }

    if ( header[0] != 'B' || header[1] != 'M' ) {
        return -1;
    }

    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);

    if (imageSize == 0)    imageSize=width*height*3;
    if (dataPos == 0)      dataPos=54;

    bufferptr = new unsigned char [imageSize];

    int retcode = fread(bufferptr, 1, imageSize, file);
    fclose(file);
}