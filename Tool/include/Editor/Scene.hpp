#pragma once

#include "Entity/Entity.hpp"
#include <entt/entt.hpp>
namespace MEngine
{
class Scene : public Entity
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