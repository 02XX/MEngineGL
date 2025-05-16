#pragma once
#include "Entity/Entity.hpp"
#include "IManager.hpp"
#include "Manager/IManager.hpp"
#include <unordered_map>
namespace MEngine
{
template <typename TEntity>
    requires std::derived_from<TEntity, IEntity>
class Manager : public IManager<TEntity>
{
  protected:
    std::unordered_map<UUID, std::shared_ptr<TEntity>> mEntities;

  public:
    virtual ~Manager() = default;

    // CRUD
    virtual std::shared_ptr<TEntity> GetById(const UUID &id) override
    {
        auto it = mEntities.find(id);
        if (it != mEntities.end())
        {
            return it->second;
        }
        return nullptr;
    }
    virtual std::vector<std::shared_ptr<TEntity>> GetAll() override
    {
        std::vector<std::shared_ptr<TEntity>> entities;
        for (const auto &pair : mEntities)
        {
            entities.push_back(pair.second);
        }
        return entities;
    }
    virtual void Delete(const UUID &id) override
    {
        auto it = mEntities.find(id);
        if (it != mEntities.end())
        {
            mEntities.erase(it);
        }
    }
};
} // namespace MEngine