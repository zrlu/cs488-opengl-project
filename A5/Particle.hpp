#pragma once

#include <glm/glm.hpp>
#include <imgui/imgui.h>

using namespace glm;
using namespace std;

class Particle
{
  public:
    float elapsedTime;
    glm::vec3 position;
    glm::vec3 velocity;
    float gravityEffect;
    float lifeLength;
    float rotation;
    float scale;

    Particle(
        vec3 position,
        vec3 velocity,
        float gravityEffect,
        float lifeLength,
        float rotation,
        float scale);
    ~Particle();

    bool update();
};