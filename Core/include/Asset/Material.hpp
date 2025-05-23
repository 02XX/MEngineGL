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
  public:
    PipelineType PipelineType{PipelineType::None};

  public:
    ~Material() override = default;
};
} // namespace MEngine

namespace nlohmann
{

template <> struct adl_serializer<MEngine::Material>
{
    static void to_json(json &j, const MEngine::Material &material)
    {
        j = static_cast<MEngine::Asset>(material);
        j["PipelineType"] = magic_enum::enum_name(material.PipelineType);
    }
    static void from_json(const json &j, MEngine::Material &material)
    {
        static_cast<MEngine::Asset &>(material) = j;
        material.PipelineType = magic_enum::enum_cast<MEngine::PipelineType>(j.at("PipelineType")).value();
    }
};

} // namespace nlohmann
