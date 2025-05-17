#pragma once
#include "Entity/IEntity.hpp"
#include <glad/glad.h>
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
    virtual const UUID &GetPipelineID() const = 0;

    // setters
    virtual void SetMaterialType(MaterialType type) = 0;
};
} // namespace MEngine