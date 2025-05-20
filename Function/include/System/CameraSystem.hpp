#pragma once
#include "Component/CameraComponent.hpp"
#include "Math.hpp"
#include "System/System.hpp"
#include <entt/entt.hpp>
#include <memory>

namespace MEngine
{
class CameraSystem final : public System
{
  private:
  public:
    CameraSystem(std::shared_ptr<entt::registry> registry, std::shared_ptr<ResourceManager> resourceManager)
        : System(registry, resourceManager)
    {
    }
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

  private:
    void CalculateMatrix(entt::entity entity);
};
} // namespace MEngine