#pragma once
#include "Asset/Asset.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>

namespace MEngine
{
enum class PipelineType
{
    ForwardOpaquePBR,
    ForwardOpaquePhong,
    TransparentPBR,
    TransparentPhong,
    Custom
};
class Pipeline final : public Asset
{
public:
    std::filesystem::path VertexShaderPath{};
    std::filesystem::path FragmentShaderPath{};
    std::filesystem::path GeometryShaderPath{};
    bool blendingEnabled = false;
    GLenum blendSrc = GL_SRC_ALPHA;
    GLenum blendDest = GL_ONE_MINUS_SRC_ALPHA;
    //TODO: More Pipeline Settings
private:
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint geometryShader = 0;
    GLuint program = 0;

  public:
    Pipeline();
    ~Pipeline() override;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Pipeline>
{
    static void to_json(json &j, const MEngine::Pipeline &shader)
    {
        j = static_cast<MEngine::Asset>(shader);
        j["vertexShaderPath"] = shader.VertexShaderPath.string();
        j["fragmentShaderPath"] = shader.FragmentShaderPath.string();
        j["geometryShaderPath"] = shader.GeometryShaderPath.string();
    }
    static void from_json(const json &j, MEngine::Pipeline &shader)
    {
        static_cast<MEngine::Asset &>(shader) = j.get<MEngine::Asset>();
        shader.VertexShaderPath = j.at("vertexShaderPath").get<std::string>();
        shader.FragmentShaderPath = j.at("fragmentShaderPath").get<std::string>();
        shader.GeometryShaderPath = j.at("geometryShaderPath").get<std::string>();
    }
};
} // namespace nlohmann
