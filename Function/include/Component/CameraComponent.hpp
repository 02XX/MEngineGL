#pragma once
#include "Component/Component.hpp"
#include "Math.hpp"

namespace MEngine
{
struct CameraComponent : public Component
{
    glm::vec3 target = {0.0f, 0.0f, 0.0f};
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    float fovY = 60;
    float zoom = 1.0f;
    float fovX = 60;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float aspectRatio = 16.0f / 9.0f;

    bool isMainCamera = false;
    bool isEditorCamera = false;
    glm::mat4 viewMatrix = glm::identity<glm::mat4>();
    glm::mat4 projectionMatrix = glm::identity<glm::mat4>();
};
} // namespace MEngine