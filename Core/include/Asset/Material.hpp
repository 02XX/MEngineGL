#pragma once
#include "Asset/Asset.hpp"
#include "Asset/Pipeline.hpp"
#include "UUID.hpp"
#include <glad/glad.h>
#include <glm/vec4.hpp>
#include <magic_enum/magic_enum.hpp>

namespace MEngine
{
// 抽象类
class Material : public Asset
{
  private:
    // 索引即为binding号
    std::vector<GLuint> UBOs;

  public:
    // 索引即为bingding号
    std::vector<UUID> textures;
    PipelineType PipelineType{PipelineType::ForwardOpaquePBR};
    UUID PipelineID{};

  public:
    Material();
    ~Material() override;
};
} // namespace MEngine

namespace nlohmann
{

template <> struct adl_serializer<MEngine::Material>
{
    static void to_json(json &j, const MEngine::Material &material)
    {
        j = static_cast<MEngine::Asset>(material);
        j["textures"] = material.textures;
        j["PipelineType"] = magic_enum::enum_name(material.PipelineType);
        j["PipelineID"] = material.PipelineID;
    }
    static void from_json(const json &j, MEngine::Material &material)
    {
        static_cast<MEngine::Asset &>(material) = j;
        material.textures = j.at("textures").get<std::vector<MEngine::UUID>>();
        material.PipelineType = magic_enum::enum_cast<MEngine::PipelineType>(j.at("PipelineType")).value();
        material.PipelineID = j.at("PipelineID").get<MEngine::UUID>();
    }
};

} // namespace nlohmann
