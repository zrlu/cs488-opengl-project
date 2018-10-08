#include "ParticleMaster.hpp"

#include <iostream>

std::list<Particle*> ParticleMaster::particles = std::list<Particle*>();

unsigned int ParticleMaster::num_particles = 0;

void ParticleMaster::update()
{
    Particle *p;
    auto it = ParticleMaster::particles.begin();
    while (it != ParticleMaster::particles.end())
    {
        p = *it;
        if (!p->update())
        {
            it = ParticleMaster::particles.erase(it);
            --ParticleMaster::num_particles;
            delete p;
        }
        it++;
    }
} 

void ParticleMaster::addParticle(Particle *particle)
{
    ParticleMaster::particles.push_back(particle);
    ++ParticleMaster::num_particles;
}