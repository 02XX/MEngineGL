#pragma once
#include "UUID.hpp"
namespace MEngine
{
class Component
{
  public:
    virtual ~Component() = default;
    bool dirty = false;
};
} // namespace MEngine
