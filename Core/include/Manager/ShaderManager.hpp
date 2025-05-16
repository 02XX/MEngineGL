#pragma once
#include "Entity/IShader.hpp"
#include "Entity/Shader.hpp"
#include "Logger.hpp"
#include "Manager/Manager.hpp"

namespace MEngine
{
class ShaderManager final : Manager<Shader>
{
    std::shared_ptr<Shader> Create() override;
    void Update(const UUID &id, const Shader &entity) override;
    GLuint LoadShader(const std::filesystem::path &shaderPath);
};
} // namespace MEngine