#pragma once

namespace MEngine
{
class ISystem
{
  public:
    virtual ~ISystem() = default;

    virtual void Init() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() = 0;
};
} // namespace MEngine