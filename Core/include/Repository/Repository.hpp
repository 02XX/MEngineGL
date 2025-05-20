#pragma once
#include "Repository/IRepository.hpp"

namespace MEngine
{
template <typename TEntity>
    requires std::derived_from<TEntity, Entity>
class Repository : public IRepository<TEntity>
{
  public:
    virtual ~Repository() = default;
    virtual std::shared_ptr<TEntity> Create() override
    {
        auto entity = std::make_shared<TEntity>();
        return entity;
    }
};
template <typename T> struct RepositoryTraits
{
    using RepositoryType = Repository<T>;
};
} // namespace MEngine