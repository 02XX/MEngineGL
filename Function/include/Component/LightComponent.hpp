#pragma once
#include <glm/vec3.hpp>
#include "Component.hpp"
namespace MEngine
{
    enum class LightType
    {
        Directional,
        Point,
        Spot,
    };
    struct LightComponent final : public MEngine::Component{
        LightType LightType = LightType::Directional;
        float Intensity = 1.0f;
        glm::vec3 Color = glm::vec3(1.0f);
        //point
        float Radius = 10.0f;
    };
}