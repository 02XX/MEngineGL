#pragma once

#include "Component/CameraComponent.hpp"
#include "Component/MaterialComponent.hpp"
#include "Component/MeshComponent.hpp"
#include "Component/TransformComponent.hpp"
#include "ResourceManager.hpp"
#include "System/System.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace MEngine
{
class RenderSystem final : public System
{
  private:
    CameraComponent mMainCamera;
    std::unordered_map<UUID, std::vector<entt::entity>> mRenderQueue;

  public:
    GLuint FBO = 0;
    GLuint ColorAttachment;
    GLuint DepthAttachment;
    struct Light
    {
      glm::vec3 position;
      glm::vec3 direction;
      glm::vec3 color;
      float intensity;
      float radius;
      int type;
    };
    GLuint LightUBO;
  public:
    RenderSystem(std::shared_ptr<entt::registry> registry, std::shared_ptr<ResourceManager> resourceManager);
    ~RenderSystem();

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    void GetMainCamera();
    void CreateFrameBuffer(int width = 1280, int height = 720);
    void UpdateSource();

    void RenderQueue();
    void RenderShadowPass();
    void RenderDeferredPass();
    void RenderForwardPass();
    void RenderPostProcessPass();
};
} // namespace MEngine