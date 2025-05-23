#pragma once
#include "Asset/Asset.hpp"
#include "Material.hpp"
#include <glm/vec4.hpp>
#include "UUID.hpp"
#include "Asset/Pipeline.hpp"
#include <magic_enum/magic_enum.hpp>

namespace MEngine
{
//抽象类
class Material : public Asset
{
  private:
    //索引即为binding号
    std::vector<GLuint> UBOs;
  public:
    //索引即为bingding号
    std::vector<UUID> textures;
    PipelineType PipelineType{PipelineType::ForwardOpaquePBR};
    UUID PipelineID{};

  public:
    Material();
    ~Material() override = 0;
};
} // namespace MEngine

namespace nlohmann
{

template <> struct adl_serializer<MEngine::Material>
{
    static void to_json(json &j, const MEngine::Material &material)
    {
        j = static_cast<MEngine::Asset>(material);
        j["materialType"] = magic_enum::enum_name(material.MaterialType);
        j["pipelineID"] = material.PipelineID.ToString();
        j["color"] = {material.color.r, material.color.g, material.color.b, material.color.a};
    }
    static void from_json(const json &j, MEngine::Material &material)
    {
        static_cast<MEngine::Asset &>(material) = j;
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
        auto color = j.at("color").get<std::vector<float>>();
        if (color.size() == 4)
        {
            material.color = glm::vec4(color[0], color[1], color[2], color[3]);
        }
        else
        {
            throw std::runtime_error("Invalid color format");
        }
    }
};

} // namespace nlohmann
