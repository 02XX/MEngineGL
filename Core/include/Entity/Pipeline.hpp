#pragma once
#include "Entity/Entity.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>

namespace MEngine
{
class Pipeline final : public Entity
{
  public:
    std::filesystem::path VertexShaderPath{};
    std::filesystem::path FragmentShaderPath{};
    std::filesystem::path GeometryShaderPath{};

    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;
    GLuint geometryShaderID = 0;
    GLuint programID = 0;

  public:
    Pipeline();
    ~Pipeline() override;
    GLuint LoadShader(GLuint shaderID, const std::filesystem::path &shaderPath);
    void Update() override;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Pipeline>
{
    static void to_json(json &j, const MEngine::Pipeline &shader)
    {
        j = static_cast<MEngine::Entity>(shader);
        j["vertexShaderPath"] = shader.VertexShaderPath.string();
        j["fragmentShaderPath"] = shader.FragmentShaderPath.string();
        j["geometryShaderPath"] = shader.GeometryShaderPath.string();
    }
    static void from_json(const json &j, MEngine::Pipeline &shader)
    {
        static_cast<MEngine::Entity &>(shader) = j.get<MEngine::Entity>();
        shader.VertexShaderPath = j.at("vertexShaderPath").get<std::string>();
        shader.FragmentShaderPath = j.at("fragmentShaderPath").get<std::string>();
        shader.GeometryShaderPath = j.at("geometryShaderPath").get<std::string>();
    }
};
} // namespace nlohmann
