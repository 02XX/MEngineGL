#pragma once
#include "Entity.hpp"
#include <filesystem>
#include <glad/glad.h>
namespace MEngine
{
class IShader : virtual public IEntity
{
  public:
    virtual ~IShader() = default;

    // getters
    virtual const GLuint &GetVertexShader() const = 0;
    virtual const GLuint &GetFragmentShader() const = 0;
    virtual const GLuint &GetGeometryShader() const = 0;
    virtual const GLuint &GetTessControlShader() const = 0;
    virtual const GLuint &GetTessEvalShader() const = 0;
    virtual const GLuint &GetComputeShader() const = 0;

    // setters
    virtual void SetVertexShader(const std::filesystem::path &vertexShader) = 0;
    virtual void SetFragmentShader(const std::filesystem::path &fragmentShader) = 0;
    virtual void SetGeometryShader(const std::filesystem::path &geometryShader) = 0;
    virtual void SetTessControlShader(const std::filesystem::path &tessControlShader) = 0;
    virtual void SetTessEvalShader(const std::filesystem::path &tessEvalShader) = 0;
    virtual void SetComputeShader(const std::filesystem::path &computeShader) = 0;
};
} // namespace MEngine