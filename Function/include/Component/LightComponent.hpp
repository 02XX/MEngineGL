#pragma once
#include "Component.hpp"
#include <glm/vec3.hpp>

namespace MEngine
{
namespace Function
{
enum class LightType
{
    Directional,
    Point,
    Spot,
};
struct LightComponent final : public Component
{
    LightType LightType = LightType::Directional;
    float Intensity = 1.0f;
    glm::vec3 Color = glm::vec3(1.0f);
    // point
    float Radius = 10.0f;
};
} // namespace Function
} // namespace MEngine