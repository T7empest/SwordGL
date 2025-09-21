//
// Created by Lukáš Blažek on 06.09.2025.
//

#include "particle_system.h"

#include <random>

#include "../core/sword_app.h"
#include "../core/utils.h"

void ParticleSystem::emit(float dt, glm::vec2 pos)
{
	Particle particle = {
		.position = pos,
		.scale = 32.0f,
		.color = utils::randomColor4f(),
		.velocity = utils::randomDirection2D(20.0f),
		.acceleration = utils::randomDirection2D(9.0f),
		.gravity = 2.0f
	};
	particles_.push_back(particle);
	std::cout << "added an entity: " << std::endl;
	std::cout << particles_.size() << std::endl;;
	std::cout << "X: " << particle.position.x << std::endl;
	std::cout << "Y: " << particle.position.y << std::endl;
}

void ParticleSystem::update(float dt)
{
	for (size_t i = 0; i < particles_.size(); )
	{
		auto& particle = particles_[i];

		particle.position += particle.velocity * dt;
		particle.velocity += particle.acceleration * dt;
		particle.lifetime -= dt;

		// check lifetime
		if (particle.lifetime <= 0.0f)
		{
			particles_[i] = particles_.back();
			particles_.pop_back();
			std::cout << "removed an entity - lifetime expired" << std::endl;
		}
		else
		{
			++i;
		}
	}
}

std::vector<Particle>* ParticleSystem::get_particles()
{
	return &particles_;
}
