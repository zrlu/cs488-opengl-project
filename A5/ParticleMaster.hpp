#pragma once
#include "Particle.hpp"

#include <list>

class ParticleMaster
{
public:
    static std::list<Particle*> particles;
    static unsigned int num_particles;
    static void update();

    void renderParticles();

    static void addParticle(Particle *particle);
};