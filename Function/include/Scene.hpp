#pragma once
#include "Asset/Asset.hpp"
#include <entt/fwd.hpp>
namespace MEngine
{
class Scene : public Asset
{
  private:
    std::shared_ptr<entt::registry> mRegistry;

  public:
    inline std::shared_ptr<entt::registry> GetRegistry() const
    {
        return mRegistry;
    }
    inline void SetRegistry(const std::shared_ptr<entt::registry> registry)
    {
        mRegistry = registry;
    }
};
} // namespace MEngine