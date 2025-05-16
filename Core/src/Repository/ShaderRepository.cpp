#include "Repository/ShaderRepository.hpp"

namespace MEngine
{
void ShaderRepository::Update(std::shared_ptr<Shader> entity)
{
    // 更新实体
    entity->SetVertexShader(entity->GetVertexShaderPath());
    entity->SetFragmentShader(entity->GetFragmentShaderPath());
    entity->SetGeometryShader(entity->GetGeometryShaderPath());
}
} // namespace MEngine