#include "System/TransformSystem.hpp"

namespace MEngine
{
void TransformSystem::Init()
{
}
void TransformSystem::Update(float deltaTime)
{
    auto view = mRegistry->view<TransformComponent>();
    for (auto entity : view)
    {
        auto &transformComponent = view.get<TransformComponent>(entity);
        if (transformComponent.dirty)
        {
            CalculateMatrix(entity);
            transformComponent.dirty = false;
        }
    }
}
void TransformSystem::Shutdown()
{
}
void TransformSystem::CalculateMatrix(entt::entity entity)
{
    // 获取实体的TransformComponent
    auto &transformComponent = mRegistry->get<TransformComponent>(entity);
    // local
    glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), transformComponent.position) *
                            glm::mat4_cast(transformComponent.rotation) *
                            glm::scale(glm::mat4(1.0f), transformComponent.scale);
    if (transformComponent.parent != entt::null)
    {
        // 获取父实体的TransformComponent
        auto &parentTransform = mRegistry->get<TransformComponent>(transformComponent.parent);
        // 计算世界矩阵
        transformComponent.worldMatrix = parentTransform.worldMatrix * localMatrix;
    }
    else
    {
        // 如果没有父实体，则世界矩阵等于局部矩阵
        transformComponent.worldMatrix = localMatrix;
    }
    for (auto child : transformComponent.children)
    {
        auto &childTransform = mRegistry->get<TransformComponent>(child);
        CalculateMatrix(child);
    }
}
void TransformSystem::Translate(TransformComponent &transform, const glm::vec3 &delta)
{
}
void TransformSystem::Rotate(TransformComponent &transform, float angle, const glm::vec3 &axis)
{
}
void TransformSystem::Scale(TransformComponent &transform, const glm::vec3 &scale)
{
}
} // namespace MEngine