#pragma once
#include "Entity/Entity.hpp"
#include "Material.hpp"
#include "Math.hpp"
#include "UUID.hpp"
#include <magic_enum/magic_enum.hpp>
namespace MEngine
{
enum class MaterialType
{
    PBR,
    Phong
};
class Material : public Entity
{
  public:
    MaterialType MaterialType{MaterialType::PBR};
    UUID PipelineID{};

  public:
    Material();
    virtual ~Material();
    virtual void Update() override
    {
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
        j["materialType"] = magic_enum::enum_name(material.MaterialType);
        j["pipelineID"] = material.PipelineID.ToString();
    }
    static void from_json(const json &j, MEngine::Material &material)
    {
        static_cast<MEngine::Entity &>(material) = j;
        std::string materialType = j.at("materialType").get<std::string>();
        auto enumType = magic_enum::enum_cast<MEngine::MaterialType>(materialType);
        if (enumType.has_value())
        {
            material.MaterialType = enumType.value();
        }
        else
        {
            throw std::runtime_error("Invalid material type: " + materialType);
        }
        material.PipelineID = MEngine::UUID(j.at("pipelineID").get<std::string>());
    }
};

} // namespace nlohmann
