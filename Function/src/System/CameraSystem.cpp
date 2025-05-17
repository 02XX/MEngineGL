#include "System/CameraSystem.hpp"

namespace MEngine
{
void CameraSystem::Init()
{
}
void CameraSystem::Update(float deltaTime)
{
    auto view = mRegistry->view<CameraComponent>();
    for (auto entity : view)
    {
        auto &cameraComponent = view.get<CameraComponent>(entity);
        if (cameraComponent.dirty)
        {
            CalculateMatrix(entity);
            cameraComponent.dirty = false;
        }
    }
}
void CameraSystem::Shutdown()
{
}
void CameraSystem::CalculateMatrix(entt::entity entity)
{
    auto &cameraComponent = mRegistry->get<CameraComponent>(entity);
    cameraComponent.viewMatrix = glm::lookAtRH(cameraComponent.position, cameraComponent.target, cameraComponent.up);
    cameraComponent.projectionMatrix =
        glm::perspectiveRH(glm::radians(cameraComponent.fov), cameraComponent.aspectRatio, cameraComponent.nearPlane,
                           cameraComponent.farPlane);
}
} // namespace MEngine