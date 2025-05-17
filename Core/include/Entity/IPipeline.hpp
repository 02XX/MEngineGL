#pragma once
#include "Entity/IEntity.hpp"
#include <filesystem>
#include <glad/glad.h>
#include <nlohmann/json.hpp>

namespace MEngine
{
class IPipeline : virtual public IEntity
{
  public:
    virtual ~IPipeline() = default;

    // getters
    virtual const GLuint &GetVertexShader() const = 0;
    virtual const GLuint &GetFragmentShader() const = 0;
    virtual const GLuint &GetGeometryShader() const = 0;

    virtual const std::filesystem::path &GetVertexShaderPath() const = 0;
    virtual const std::filesystem::path &GetFragmentShaderPath() const = 0;
    virtual const std::filesystem::path &GetGeometryShaderPath() const = 0;
    virtual const GLuint &GetProgramID() const = 0;
    // setters
    virtual void SetVertexShader(const std::filesystem::path &vertexShader) = 0;
    virtual void SetFragmentShader(const std::filesystem::path &fragmentShader) = 0;
    virtual void SetGeometryShader(const std::filesystem::path &geometryShader) = 0;
};
} // namespace MEngine
