#pragma once
#include "ResourceManager.hpp"
#include "System/ISystem.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <memory>
namespace MEngine
{
class System : public ISystem
{
  protected:
    std::shared_ptr<entt::registry> mRegistry;
    std::shared_ptr<ResourceManager> mResourceManager;

  public:
    System(std::shared_ptr<entt::registry> registry, std::shared_ptr<ResourceManager> resourceManager)
        : mRegistry(registry), mResourceManager(resourceManager)
    {
    }
    virtual ~System() = default;
};
} // namespace MEngine