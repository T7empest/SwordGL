//
// Created by Lukáš Blažek on 21.09.2025.
//

#pragma once
#include <random>

#include "glm/detail/func_packing_simd.inl"
#include "glm/vec2.hpp"
#include <numbers>
constexpr float TWO_PI = 2.0f * std::numbers::pi_v<float>;

namespace utils
{
inline glm::vec2 randomDirection2D(float speed = 1.0f)
{
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<float> dist(3.6f, 6.0f);

    float angle = dist(rng);
    return glm::vec2(std::cos(angle), std::sin(angle)) * speed;
}

inline glm::vec4 randomColor4f()
{
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution dist(0.0f, 0.1f);

    return glm::vec4{0.2f + 3 * dist(rng), 0.2f + 3 * dist(rng),
                     0.2f + 3 * dist(rng), dist(rng)};
}

inline float randomFloat(float min, float max)
{
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution dist(min, max);

    return dist(rng);
}
} // namespace utils
