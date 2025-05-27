#pragma once
#include "IAssetManager.hpp"
#include "System/ISystem.hpp"
#include <entt/fwd.hpp>
#include <memory>

namespace MEngine
{
namespace Function
{
class System : public ISystem
{
  protected:
    std::shared_ptr<entt::registry> mRegistry;
    std::shared_ptr<IAssetManager> mAssetManager;

  public:
    System(std::shared_ptr<entt::registry> registry, std::shared_ptr<IAssetManager> assetManager)
        : mRegistry(registry), mAssetManager(assetManager)
    {
    }
    virtual ~System() = default;
};
} // namespace Function
} // namespace MEngine