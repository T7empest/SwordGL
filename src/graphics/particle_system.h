//
// Created by Lukáš Blažek on 06.09.2025.
//

#pragma once

#include "glm/glm.hpp"

struct Particle
{
	float     lifetime = 1.0f;
	glm::vec2 position;
	float     scale;
	glm::vec2 velocity;
	glm::vec2 acceleration;
	float     gravity;
};

class ParticleSystem
{
public:
	void emit(float dt, glm::vec2 pos);
	void update(float dt);

private:
	std::vector<Particle> particles_;
};
