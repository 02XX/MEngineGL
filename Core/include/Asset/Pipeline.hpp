#pragma once
#include "Asset/Asset.hpp"
#include <filesystem>
#include <glad/glad.h>
namespace MEngine
{
namespace Core
{
enum class PipelineType
{
    None,
    ForwardOpaquePBR,
    ForwardOpaquePhong,
    TransparentPBR,
    TransparentPhong,
    Custom
};
class Pipeline final : public Asset
{
  private:
    GLuint UBO;

  public:
    std::filesystem::path VertexShaderPath{};
    std::filesystem::path FragmentShaderPath{};
    std::filesystem::path GeometryShaderPath{};
    bool blendingEnabled = false;
    GLenum blendSrc = GL_SRC_ALPHA;
    GLenum blendDest = GL_ONE_MINUS_SRC_ALPHA;
    // TODO: More Pipeline Settings
  private:
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint geometryShader = 0;
    GLuint program = 0;

  public:
    ~Pipeline() override = default;
};
} // namespace Core
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Core::Pipeline>
{
    static void to_json(json &j, const MEngine::Core::Pipeline &shader)
    {
        j = static_cast<MEngine::Core::Asset>(shader);
        j["vertexShaderPath"] = shader.VertexShaderPath.string();
        j["fragmentShaderPath"] = shader.FragmentShaderPath.string();
        j["geometryShaderPath"] = shader.GeometryShaderPath.string();
    }
    static void from_json(const json &j, MEngine::Core::Pipeline &shader)
    {
        static_cast<MEngine::Core::Asset &>(shader) = j.get<MEngine::Core::Asset>();
        shader.VertexShaderPath = j.at("vertexShaderPath").get<std::string>();
        shader.FragmentShaderPath = j.at("fragmentShaderPath").get<std::string>();
        shader.GeometryShaderPath = j.at("geometryShaderPath").get<std::string>();
    }
};
} // namespace nlohmann
