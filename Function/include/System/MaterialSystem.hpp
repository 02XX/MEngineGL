#pragma once
#include "System/System.hpp"
namespace MEngine
{
namespace Function
{
class MaterialSystem final : public System
{
  public:
    MaterialSystem(std::shared_ptr<entt::registry> registry) : System(registry)
    {
    }
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
};

} // namespace Function
} // namespace MEngine