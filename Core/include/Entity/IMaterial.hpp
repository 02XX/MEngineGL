#pragma once
#include "Entity/IEntity.hpp"

namespace MEngine
{
enum class MaterialType
{
    PBR,
    Phong,
};
class IMaterial : virtual public IEntity
{
  public:
    virtual ~IMaterial() = default;
    // getters
    virtual MaterialType GetMaterialType() const = 0;

    // setters
    virtual void SetMaterialType(MaterialType type) = 0;
};
} // namespace MEngine