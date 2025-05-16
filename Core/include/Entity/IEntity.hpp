#pragma once
#include "UUID.hpp"

namespace MEngine
{
class IEntity
{
  public:
    virtual ~IEntity() = default;

    virtual const UUID &GetID() const = 0;
    virtual const std::string &GetName() const = 0;
    virtual void SetID(const UUID &id) = 0;
    virtual void SetName(const std::string &name) = 0;
};
} // namespace MEngine