#pragma once

#include "Entity/Entity.hpp"
#include "Entity/IMaterial.hpp"

namespace MEngine
{
class Material : public IMaterial, public Entity
{
  private:
    MaterialType mMaterialType = MaterialType::PBR;

  public:
    // getters
    inline MaterialType GetMaterialType() const override
    {
        return mMaterialType;
    }

    // setters
    inline void SetMaterialType(MaterialType type) override
    {
        this->mMaterialType = type;
    }
};
} // namespace MEngine