#pragma once
#include "Component.hpp"
#include "Math.hpp"
#include <entt/entt.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

namespace MEngine
{

struct TransformComponent : public Component
{
    std::string name = "None";

    glm::vec3 localPosition{0.0f, 0.0f, 0.0f};
    glm::quat localRotation = glm::identity<glm::quat>();
    glm::vec3 localScale{1.0f, 1.0f, 1.0f};
    glm::vec3 worldPosition{0.0f, 0.0f, 0.0f};
    glm::quat worldRotation = glm::identity<glm::quat>();
    glm::vec3 worldScale{1.0f, 1.0f, 1.0f};

    glm::mat4 modelMatrix = glm::identity<glm::mat4>();
    bool dirty = true;

    entt::entity parent = entt::null;
    std::vector<entt::entity> children;
};
} // namespace MEngine