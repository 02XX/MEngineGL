#pragma once
#include "Entity/IEntity.hpp"

namespace MEngine
{
class Entity : virtual public IEntity
{
  protected:
    UUID mID = UUID();
    std::string mName = "";

  public:
    Entity() = default;
    Entity(const UUID &id, const std::string &name) : mID(id), mName(name)
    {
    }
    ~Entity() override = default;

    inline const UUID &GetID() const override
    {
        return mID;
    }
    inline const std::string &GetName() const override
    {
        return mName;
    }
    inline void SetID(const UUID &id) override
    {
        this->mID = id;
    }
    inline void SetName(const std::string &name) override
    {
        this->mName = name;
    }
};
} // namespace MEngine