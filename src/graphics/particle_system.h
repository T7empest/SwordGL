//
// Created by Lukáš Blažek on 06.09.2025.
//

#pragma once

#include "glm/glm.hpp"
#include "SimpleECS.h"

struct Temporary
{
	float lifetime = 1.0f;
};

struct Transform
{
	glm::vec2 position;
	glm::vec2 scale;
};

struct RigidBody
{
	glm::vec2 velocity;
	glm::vec2 acceleration;
	float     gravity;
};

class ParticleSystem : public System
{
public:
	void emit(float dt);
};
