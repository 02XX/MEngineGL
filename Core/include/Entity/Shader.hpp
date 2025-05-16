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
    Shader() = default;

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

    inline void SetVertexShader(const std::filesystem::path &vertexShaderPath) override
    {
        mVertexShaderPath = vertexShaderPath;
    }
    inline void SetFragmentShader(const std::filesystem::path &fragmentShaderPath) override
    {
        mFragmentShaderPath = fragmentShaderPath;
    }
    inline void SetGeometryShader(const std::filesystem::path &geometryShaderPath) override
    {
        mGeometryShaderPath = geometryShaderPath;
    }
    inline void SetTessControlShader(const std::filesystem::path &tessControlShaderPath) override
    {
        mTessControlShaderPath = tessControlShaderPath;
    }
    inline void SetTessEvalShader(const std::filesystem::path &tessEvalShaderPath) override
    {
        mTessEvalShaderPath = tessEvalShaderPath;
    }
    inline void SetComputeShader(const std::filesystem::path &computeShaderPath) override
    {
        mComputeShaderPath = computeShaderPath;
    }
};
} // namespace MEngine