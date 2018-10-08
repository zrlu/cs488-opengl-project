#pragma once

#include <stdio.h>

int loadBMP(const char *imagepath, unsigned char *&bufferptr, unsigned int &width, unsigned int &height);
