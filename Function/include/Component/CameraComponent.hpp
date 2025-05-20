#pragma once
#include "Component/Component.hpp"
#include "Math.hpp"

namespace MEngine
{
struct CameraComponent : public Component
{
    glm::vec3 position = {2.0f, 2.0f, 5.0f};
    glm::vec3 target = {0.0f, 0.0f, 0.0f};
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float aspectRatio = 16.0f / 9.0f;

    bool isMainCamera = false;
    glm::mat4 viewMatrix = glm::identity<glm::mat4>();
    glm::mat4 projectionMatrix = glm::identity<glm::mat4>();
};
} // namespace MEngine