#pragma once
#include "Entity/Entity.hpp"
#include "Entity/IMaterial.hpp"
#include "Entity/IPipeline.hpp"
#include "Material.hpp"
#include "Math.hpp"
#include <magic_enum/magic_enum.hpp>
namespace MEngine
{
class Material : public IMaterial, public Entity
{
    friend class MaterialManager;
    friend struct nlohmann::adl_serializer<MEngine::Material>;

  protected:
    MaterialType mMaterialType = MaterialType::PBR;

    UUID mPipelineID = UUID();

  public:
    Material();
    virtual ~Material();
    // getters
    inline MaterialType GetMaterialType() const override
    {
        return MaterialType::PBR;
    }
    inline const UUID &GetPipelineID() const override
    {
        return mPipelineID;
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
        j["materialType"] = magic_enum::enum_name(material.mMaterialType);
        j["pipelineID"] = material.mPipelineID.ToString();
    }
    static void from_json(const json &j, MEngine::Material &material)
    {
        static_cast<MEngine::Entity &>(material) = j;
        std::string materialType = j.at("materialType").get<std::string>();
        auto enumType = magic_enum::enum_cast<MEngine::MaterialType>(materialType);
        if (enumType.has_value())
        {
            material.mMaterialType = enumType.value();
        }
        else
        {
            throw std::runtime_error("Invalid material type: " + materialType);
        }
        material.mPipelineID = MEngine::UUID(j.at("pipelineID").get<std::string>());
    }
};

} // namespace nlohmann
