#pragma once
#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "UUID.hpp"
#include <entt/entt.hpp>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace MEngine
{
class SceneManager
{
  private:
    // DI
    std::shared_ptr<entt::registry> mRegistry;
    std::shared_ptr<ResourceManager> mResourceManager;

  private:
    std::unordered_map<UUID, Scene> mScenes;

  public:
    SceneManager(std::shared_ptr<entt::registry> registry, std::shared_ptr<ResourceManager> resourceManager);

    void LoadScene(const UUID &sceneID);

    void SerializeScene(const std::filesystem::path &scenePath);
    void DeserializeScene(const std::filesystem::path &scenePath);
};
} // namespace MEngine