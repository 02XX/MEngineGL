#pragma once
#include "Asset.hpp"
#include <entt/entt.hpp>
#include <glm/vec3.hpp>

namespace MEngine
{
class Scene final: public Asset
{
    public:
        glm::vec3 GlobalGravity;
        entt::entity MainCamera;
        entt::registry Registry;
};
}
