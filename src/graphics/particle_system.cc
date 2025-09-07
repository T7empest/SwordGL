//
// Created by Lukáš Blažek on 06.09.2025.
//

#include "particle_system.h"
#include "../core/sword_app.h"

extern ComponentController controller;

void ParticleSystem::emit(float dt)
{
	std::vector<Entity> to_remove{};

	for (const auto& entity : entities_)
	{
		auto& rigid_body = controller.GetComponent<RigidBody>(entity);
		auto& transform  = controller.GetComponent<Transform>(entity);
		auto& temporary  = controller.GetComponent<Temporary>(entity);

		temporary.lifetime -= dt;
		if (temporary.lifetime <= 0)
		{
			to_remove.push_back(entity);
		}
		transform.position += rigid_body.velocity * dt;
	}

	for (auto& entity : to_remove)
	{
		controller.DestroyEntity(entity);
	}
}
