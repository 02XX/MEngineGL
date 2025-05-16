#pragma once

#include "Entity/Entity.hpp"
#include "Entity/IMaterial.hpp"
#include "Entity/IShader.hpp"
#include <magic_enum/magic_enum.hpp>
namespace MEngine
{
class Material : public IMaterial, public Entity
{
    friend class MaterialManager;

  protected:
    MaterialType mMaterialType = MaterialType::PBR;
    GLuint mShaderProgramID = 0;

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
        static_cast<MEngine::Entity>(material) = j;
        std::string materialTypeStr = j.at("materialType").get<std::string>();
        auto materialType = magic_enum::enum_cast<MEngine::MaterialType>(materialTypeStr).value();
        material.SetMaterialType(materialType);
    }
};
} // namespace nlohmann