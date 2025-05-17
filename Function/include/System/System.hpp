#pragma once
#include "System/ISystem.hpp"
#include <entt/entt.hpp>
namespace MEngine
{
class System : public ISystem
{
  protected:
    std::shared_ptr<entt::registry> mRegistry;

  public:
    System(const std::shared_ptr<entt::registry> &registry) : mRegistry(registry)
    {
    }
    virtual ~System() = default;
};
} // namespace MEngine