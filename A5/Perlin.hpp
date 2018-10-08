#pragma once

class Perlin
{
    int p[512];
public:
    Perlin();
    ~Perlin();
    double lerp(double t, double a, double b);
    double fade(double t);
    double grad(int hash, double x, double y, double z);
    double grad(int hash, double x, double y);
    double noise(double x, double y, double z);
    double noise(double x, double y);
    double octaveNoise(double x, double y, double z, unsigned int octaves);
    double octaveNoise(double x, double y, unsigned int octaves);
};