#pragma once
#include <refl.hpp>
namespace MEngine
{
class Component
{
  public:
    virtual ~Component() = default;
    bool dirty = false;
};
} // namespace MEngine

REFL_AUTO(type(MEngine::Component), field(dirty))