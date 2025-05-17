#pragma once
#include "Entity/IEntity.hpp"
namespace MEngine
{
class ITexture : virtual public IEntity
{
  public:
    virtual ~ITexture() = default;
    virtual void SetImagePath(const std::filesystem::path &path) = 0;
    virtual const std::filesystem::path &GetImagePath() const = 0;
    virtual const uint32_t &GetWidth() const = 0;
    virtual const uint32_t &GetHeight() const = 0;
};
} // namespace MEngine