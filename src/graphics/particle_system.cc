//
// Created by Lukáš Blažek on 06.09.2025.
//

#include "particle_system.h"
#include "../core/sword_app.h"

void ParticleSystem::emit(float dt, glm::vec2 pos)
{
	Particle particle = {
		.position = pos,
		.scale = 2.0f,
		.velocity = glm::vec2(2.0f, 2.0f),
		.acceleration = glm::vec2(2.0f, 2.0f),
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
