#pragma once
#include "Entity/IEntity.hpp"
#include <memory>
namespace MEngine
{
template <typename TEntity>
    requires std::derived_from<TEntity, IEntity>
class IRepository
{
  public:
    virtual ~IRepository() = default;
    virtual std::shared_ptr<TEntity> Create() = 0;
    virtual void Update(std::shared_ptr<TEntity> entity) = 0;
};
} // namespace MEngine