#pragma once
#include "Entity/IEntity.hpp"
#include "UUID.hpp"

namespace MEngine
{
template <typename TEntity>
    requires std::derived_from<TEntity, IEntity>
class IManager
{
  public:
    virtual ~IManager() = default;
    // CRUD
    virtual std::shared_ptr<TEntity> Create() = 0;
    virtual std::shared_ptr<TEntity> GetById(const UUID &id) = 0;
    virtual std::vector<std::shared_ptr<TEntity>> GetAll() = 0;
    virtual void Update(const UUID &id, const TEntity &entity) = 0;
    virtual void Delete(const UUID &id) = 0;
};
} // namespace MEngine