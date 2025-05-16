#pragma once
#include "Entity/IShader.hpp"
#include "Entity/Shader.hpp"
#include "Logger.hpp"
#include "Manager/Manager.hpp"

namespace MEngine
{
class ShaderManager final : public Manager<Shader>
{
  public:
    std::shared_ptr<Shader> Create() override;
    void Update(const UUID &id, const Shader &entity) override;
    GLuint LoadShader(GLuint shaderID, const std::filesystem::path &shaderPath);
    UUID Load(const std::filesystem::path &path) override;
    void Save(const std::filesystem::path &path, const Shader &entity) override;
};
} // namespace MEngine