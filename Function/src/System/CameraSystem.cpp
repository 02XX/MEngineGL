#include "System/CameraSystem.hpp"
#include "Component/CameraComponent.hpp"
#include "Component/TransformComponent.hpp"
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace MEngine
{
void CameraSystem::Init()
{
}
void CameraSystem::Update(float deltaTime)
{
    auto view = mRegistry->view<CameraComponent, TransformComponent>();
    for (auto entity : view)
    {
        auto &cameraComponent = view.get<CameraComponent>(entity);
        auto &transformComponent = view.get<TransformComponent>(entity);
        auto &camera = view.get<CameraComponent>(entity);
        glm::mat4 viewMatrix = glm::lookAtRH(transformComponent.worldPosition, camera.target, camera.up);
        auto rotationMatrix = glm::mat4_cast(transformComponent.worldRotation);
        camera.viewMatrix = rotationMatrix * viewMatrix;
        camera.projectionMatrix = glm::perspective(glm::radians(camera.fovY / camera.zoom), camera.aspectRatio,
                                                   camera.nearPlane, camera.farPlane);
    }
}
void CameraSystem::Shutdown()
{
}
} // namespace MEngine