#pragma once
#include "Component/MaterialComponent.hpp"
#include "Component/MeshComponent.hpp"
#include "Component/TransformComponent.hpp"
#include "Math.hpp"
#include "System/System.hpp"
#include <entt/entt.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>

namespace MEngine
{
class TransformSystem final : public System
{
  private:
  public:
    TransformSystem(std::shared_ptr<entt::registry> registry, std::shared_ptr<ResourceManager> resourceManager)
        : System(registry, resourceManager)
    {
    }
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    static void Translate(TransformComponent &transform, const glm::vec3 &delta);
    static void Rotate(TransformComponent &transform, float angle, const glm::vec3 &axis);
    static void Scale(TransformComponent &transform, const glm::vec3 &scale);
    static void SetWorldPosition(TransformComponent &transform, const glm::vec3 &position);
    static void SetWorldRotation(TransformComponent &transform, const glm::quat &rotation);
    static void SetWorldScale(TransformComponent &transform, const glm::vec3 &scale);
    static void SetModelMatrix(TransformComponent &transform, const glm::mat4 &modelMatrix,
                               const glm::mat4 &parentMatrix = glm::mat4(1.0f));

  private:
    void CalculateMatrix(entt::entity entity);
};
} // namespace MEngine