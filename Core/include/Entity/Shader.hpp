#pragma once
#include "Entity/Entity.hpp"
#include "Entity/IShader.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>

namespace MEngine
{
class Shader final : public Entity, public IShader
{
    friend class ShaderRepository;
    friend struct nlohmann::adl_serializer<MEngine::Shader>;

  private:
    std::filesystem::path mVertexShaderPath = std::filesystem::path();
    std::filesystem::path mFragmentShaderPath = std::filesystem::path();
    std::filesystem::path mGeometryShaderPath = std::filesystem::path();

    GLuint mVertexShaderID = 0;
    GLuint mFragmentShaderID = 0;
    GLuint mGeometryShaderID = 0;

  public:
    Shader();
    ~Shader();

    inline const GLuint &GetVertexShader() const override
    {
        return mVertexShaderID;
    }
    inline const GLuint &GetFragmentShader() const override
    {
        return mFragmentShaderID;
    }
    inline const GLuint &GetGeometryShader() const override
    {
        return mGeometryShaderID;
    }
    inline const std::filesystem::path &GetVertexShaderPath() const override
    {
        return mVertexShaderPath;
    }
    inline const std::filesystem::path &GetFragmentShaderPath() const override
    {
        return mFragmentShaderPath;
    }
    inline const std::filesystem::path &GetGeometryShaderPath() const override
    {
        return mGeometryShaderPath;
    }
    void SetVertexShader(const std::filesystem::path &vertexShaderPath) override;
    void SetFragmentShader(const std::filesystem::path &fragmentShaderPath) override;
    void SetGeometryShader(const std::filesystem::path &geometryShaderPath) override;
    GLuint LoadShader(GLuint shaderID, const std::filesystem::path &shaderPath);
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Shader>
{
    static void to_json(json &j, const MEngine::Shader &shader)
    {
        j = static_cast<MEngine::Entity>(shader);
        j["vertexShaderPath"] = shader.GetVertexShaderPath().string();
        j["fragmentShaderPath"] = shader.GetFragmentShaderPath().string();
        j["geometryShaderPath"] = shader.GetGeometryShaderPath().string();
    }
    static void from_json(const json &j, MEngine::Shader &shader)
    {
        static_cast<MEngine::Entity &>(shader) = j.get<MEngine::Entity>();
        shader.SetVertexShader(j.at("vertexShaderPath").get<std::string>());
        shader.SetFragmentShader(j.at("fragmentShaderPath").get<std::string>());
        shader.SetGeometryShader(j.at("geometryShaderPath").get<std::string>());
    }
};
} // namespace nlohmann
