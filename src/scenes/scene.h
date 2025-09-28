//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once
#include "glm/vec2.hpp"

#include <array>

namespace Scene
{
struct Vertex
{
    float x, y, z;
    float r, g, b, a;
};

inline constexpr std::array vertices{
    // první trojúhelník
    Vertex{-1.0f, -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f}, // bottom-left  (black)
    Vertex{ 1.0f, -1.0f, 0.0f, 1.0f,  0.5f, 0.5f, 0.5f}, // bottom-right (dark gray)
    Vertex{ 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f}, // top-right    (white)

    // druhý trojúhelník
    Vertex{-1.0f, -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f}, // bottom-left  (black)
    Vertex{ 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f}, // top-right    (white)
    Vertex{-1.0f,  1.0f, 0.0f, 1.0f,  0.7f, 0.7f, 0.7f}  // top-left     (light gray)
};

struct UniformBuffer
{
    glm::vec2 resolution;
    float time;
    float _pad;
};

} // namespace Scene
