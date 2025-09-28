//
// Created by Lukáš Blažek on 06.09.2025.
//

#include "graphics/particle_system.h"

#include <random>

#include "core/sword_app.h"
#include "core/utils.h"

void ParticleSystem::emit(float dt, glm::vec2 pos)
{
    Particle particle = {.position = pos,
                         .scale = utils::randomFloat(4.f, 1155.f),
                         .lifetime = utils::randomFloat(1.f, 10.f),
                         .color = utils::randomColor4f(),
                         .velocity = utils::randomDirection2D(100.0f),
                         .acceleration = utils::randomDirection2D(62.0f),
                         .gravity = -8.0f};
    particles_.push_back(particle);
}

void ParticleSystem::update(float dt)
{
    for (size_t i = 0; i < particles_.size();)
    {
        auto &particle = particles_[i];

        particle.position += particle.velocity * dt;
        particle.velocity += particle.acceleration * dt;
        particle.lifetime -= 10 * dt;

        // check lifetime
        if (particle.lifetime <= 1.0f)
        {
            particles_[i] = particles_.back();
            particles_.pop_back();
        }
        else { ++i; }
    }
}

std::vector<Particle> *ParticleSystem::get_particles() { return &particles_; }
