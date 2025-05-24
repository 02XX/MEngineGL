#pragma once
#include "System/ISystem.hpp"
#include <entt/fwd.hpp>
#include <memory>
namespace MEngine
{
class System : public ISystem
{
  protected:
    std::shared_ptr<entt::registry> mRegistry;

  public:
    System(std::shared_ptr<entt::registry> registry) : mRegistry(registry)
    {
    }
    virtual ~System() = default;
};
} // namespace MEngine