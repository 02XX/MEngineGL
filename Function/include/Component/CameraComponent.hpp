#pragma once
#include "Component/Component.hpp"
#include "Math.hpp"

namespace MEngine
{
struct CameraComponent : public Component
{
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float nearPlane;
    float farPlane;
    float aspectRatio;

    bool isMainCamera = false;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};
} // namespace MEngine