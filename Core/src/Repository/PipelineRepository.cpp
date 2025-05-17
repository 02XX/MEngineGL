#include "Repository/PipelineRepository.hpp"

namespace MEngine
{
void PipelineRepository::Update(std::shared_ptr<Pipeline> entity)
{
    // 更新实体
    entity->Update();
}
} // namespace MEngine