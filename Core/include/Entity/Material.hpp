#pragma once
#include "Entity/Entity.hpp"
#include "Entity/IMaterial.hpp"
#include "Entity/IShader.hpp"
#include "Material.hpp"
#include "Math.hpp"

namespace MEngine
{
class Material : public IMaterial, public Entity
{
    friend class MaterialManager;

  protected:
    MaterialType mMaterialType = MaterialType::PBR;
    // shader
    UUID mShaderID = UUID();

  public:
    Material();
    virtual ~Material();
    // getters
    inline MaterialType GetMaterialType() const override
    {
        return MaterialType::PBR;
    }
    inline const UUID &GetShaderID() const override
    {
        return mShaderID;
    }

    // setters
    inline void SetMaterialType(MaterialType type) override
    {
        mMaterialType = type;
    }
};
} // namespace MEngine
