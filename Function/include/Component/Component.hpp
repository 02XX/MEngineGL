#pragma once
#include "UUID.hpp"
namespace MEngine
{
namespace Function
{
class Component
{

  public:
    virtual ~Component() = default;
    bool dirty = true;
};
} // namespace Function
} // namespace MEngine