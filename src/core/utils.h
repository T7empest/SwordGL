//
// Created by Lukáš Blažek on 21.09.2025.
//

#pragma once
#include <random>

#include "glm/vec2.hpp"
#include "glm/detail/func_packing_simd.inl"

namespace utils
{
	glm::vec2 randomDirection2D(float speed = 1.0f) {
		static std::mt19937 rng(std::random_device{}());
		static std::uniform_real_distribution<float> dist(0.0f, 2.0f * M_PI);

		float angle = dist(rng);
		return glm::vec2(std::cos(angle), std::sin(angle)) * speed;
	}

	glm::vec4 randomColor4f()
	{
		static std::mt19937 rng(std::random_device{}());
		static std::uniform_real_distribution dist(0.4f, 1.0f);

		return glm::vec4{0.0f, dist(rng), dist(rng), 1.0f};
	}
}
