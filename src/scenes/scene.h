//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once
#include <array>

namespace Scene
{
	struct Vertex
	{
		float x, y, z;
		float r, g, b, a;
	};

	inline constexpr std::array vertices
	{
		Vertex{-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.1f},
		Vertex{0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.8f},
		Vertex{0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},

		Vertex{-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
		Vertex{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
		Vertex{0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}
	};
}
