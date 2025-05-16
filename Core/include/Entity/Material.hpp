#pragma once
#include "Entity/Entity.hpp"
#include "Entity/IMaterial.hpp"
#include "Entity/IShader.hpp"
#include "Material.hpp"
#include "Math.hpp"
#include <magic_enum/magic_enum.hpp>
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

namespace nlohmann
{

template <> struct adl_serializer<MEngine::Material>
{
    static void to_json(json &j, const MEngine::Material &material)
    {
        j = static_cast<MEngine::Entity>(material);
        j["materialType"] = magic_enum::enum_name(material.GetMaterialType());
    }
    static void from_json(const json &j, MEngine::Material &material)
    {
        static_cast<MEngine::Entity &>(material) = j;
        std::string materialType = j.at("materialType").get<std::string>();
        auto enumType = magic_enum::enum_cast<MEngine::MaterialType>(materialType);
        if (enumType.has_value())
        {
            material.SetMaterialType(enumType.value());
        }
        else
        {
            throw std::runtime_error("Invalid material type: " + materialType);
        }
    }
};

} // namespace nlohmann
