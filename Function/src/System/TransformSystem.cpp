#include "System/TransformSystem.hpp"
#include "Math.hpp"

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
        CalculateMatrix(entity);
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
    glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), transformComponent.localPosition) *
                            glm::mat4_cast(transformComponent.localRotation) *
                            glm::scale(glm::mat4(1.0f), transformComponent.localScale);
    if (transformComponent.parent != entt::null)
    {
        auto &parentTransform = mRegistry->get<TransformComponent>(transformComponent.parent);
        transformComponent.modelMatrix = parentTransform.modelMatrix * localMatrix;
    }
    else
    {
        transformComponent.modelMatrix = localMatrix;
    }

    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(transformComponent.modelMatrix, transformComponent.worldScale, transformComponent.worldRotation,
                   transformComponent.worldPosition, skew, perspective);

    // 递归更新所有子节点
    for (auto child : transformComponent.children)
    {
        CalculateMatrix(child);
    }
}
void TransformSystem::Translate(TransformComponent &transform, const glm::vec3 &delta)
{
    transform.localPosition += delta;
    transform.dirty = true;
}
void TransformSystem::Rotate(TransformComponent &transform, float angle, const glm::vec3 &axis)
{
}
void TransformSystem::Scale(TransformComponent &transform, const glm::vec3 &localScale)
{
}
void TransformSystem::SetWorldPosition(TransformComponent &transform, const glm::vec3 &position)
{
}
void TransformSystem::SetWorldRotation(TransformComponent &transform, const glm::quat &rotation)
{
}
void TransformSystem::SetWorldScale(TransformComponent &transform, const glm::vec3 &scale)
{
}
void TransformSystem::SetModelMatrix(TransformComponent &transform, const glm::mat4 &modelMatrix,
                                     const glm::mat4 &parentMatrix)
{
    transform.modelMatrix = modelMatrix;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(modelMatrix, transform.worldScale, transform.worldRotation, transform.worldPosition, skew,
                   perspective);
    auto localMatrix = glm::inverse(parentMatrix) * modelMatrix;
    glm::decompose(localMatrix, transform.localScale, transform.localRotation, transform.localPosition, skew,
                   perspective);
}
} // namespace MEngine