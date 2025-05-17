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
class RenderSystem : public System
{
  private:
    std::shared_ptr<ResourceManager> mResourceManager;

  private:
    CameraComponent mMainCamera;
    std::unordered_map<UUID, std::vector<entt::entity>> mRenderQueue;

  public:
    RenderSystem(std::shared_ptr<entt::registry> registry);
    ~RenderSystem() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    void GetMainCamera();

    void RenderQueue();
    void RenderShadowPass();
    void RenderDeferredPass();
    void RenderForwardPass();
    void RenderPostProcessPass();
};
} // namespace MEngine