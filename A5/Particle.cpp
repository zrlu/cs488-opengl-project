#include "Particle.hpp"
#include "ParticleMaster.hpp"
#include <glm/gtx/io.hpp>

#include <iostream>

Particle::Particle(
        vec3 position,
        vec3 velocity,
        float gravityEffect,
        float lifeLength,
        float rotation,
        float scale) : elapsedTime(0.0),
                       position(position),
                       velocity(velocity),
                       gravityEffect(gravityEffect),
                       lifeLength(lifeLength),
                       rotation(rotation),
                       scale(scale)
    {
        ParticleMaster::addParticle(this);
    }

Particle::~Particle() {}

bool Particle::update()
{
    float invFPS = 1.0f /  ImGui::GetIO().Framerate;
    velocity.y += gravityEffect * invFPS;
    vec3 change = (invFPS) * velocity;
    position += change;
    elapsedTime = elapsedTime + invFPS;
    return elapsedTime < lifeLength;
}