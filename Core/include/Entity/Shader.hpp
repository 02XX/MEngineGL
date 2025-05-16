#pragma once
#include "Entity.hpp"
#include "Entity/IShader.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
namespace MEngine
{
class Shader : public IShader, public Entity
{
    friend class ShaderManager;
    friend struct nlohmann::adl_serializer<Shader>;

  private:
    std::filesystem::path mVertexShaderPath = std::filesystem::path();
    std::filesystem::path mFragmentShaderPath = std::filesystem::path();
    std::filesystem::path mGeometryShaderPath = std::filesystem::path();
    std::filesystem::path mTessControlShaderPath = std::filesystem::path();
    std::filesystem::path mTessEvalShaderPath = std::filesystem::path();
    std::filesystem::path mComputeShaderPath = std::filesystem::path();

    GLuint mVertexShaderID = 0;
    GLuint mFragmentShaderID = 0;
    GLuint mGeometryShaderID = 0;
    GLuint mTessControlShaderID = 0;
    GLuint mTessEvalShaderID = 0;
    GLuint mComputeShaderID = 0;

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
    inline const GLuint &GetTessControlShader() const override
    {
        return mTessControlShaderID;
    }
    inline const GLuint &GetTessEvalShader() const override
    {
        return mTessEvalShaderID;
    }
    inline const GLuint &GetComputeShader() const override
    {
        return mComputeShaderID;
    }

    void SetVertexShader(const std::filesystem::path &vertexShaderPath) override;
    void SetFragmentShader(const std::filesystem::path &fragmentShaderPath) override;
    void SetGeometryShader(const std::filesystem::path &geometryShaderPath) override;
    void SetTessControlShader(const std::filesystem::path &tessControlShaderPath) override;
    void SetTessEvalShader(const std::filesystem::path &tessEvalShaderPath) override;
    void SetComputeShader(const std::filesystem::path &computeShaderPath) override;
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
        j["vertexShaderPath"] = shader.mVertexShaderPath.string();
        j["fragmentShaderPath"] = shader.mFragmentShaderPath.string();
        j["geometryShaderPath"] = shader.mGeometryShaderPath.string();
        j["tessControlShaderPath"] = shader.mTessControlShaderPath.string();
        j["tessEvalShaderPath"] = shader.mTessEvalShaderPath.string();
        j["computeShaderPath"] = shader.mComputeShaderPath.string();
    }
    static void from_json(const json &j, MEngine::Shader &shader)
    {
        static_cast<MEngine::Entity>(shader) = j;
        shader.SetVertexShader(j.at("vertexShaderPath").get<std::string>());
        shader.SetFragmentShader(j.at("fragmentShaderPath").get<std::string>());
        shader.SetGeometryShader(j.at("geometryShaderPath").get<std::string>());
        shader.SetTessControlShader(j.at("tessControlShaderPath").get<std::string>());
        shader.SetTessEvalShader(j.at("tessEvalShaderPath").get<std::string>());
        shader.SetComputeShader(j.at("computeShaderPath").get<std::string>());
    }
};
} // namespace nlohmann