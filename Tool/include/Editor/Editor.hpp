#pragma once
#include "Component/AssestComponent.hpp"
#include "IConfigure.hpp"
#include "Logger.hpp"
#include "ResourceManager.hpp"
#include "SceneManager.hpp"
#include "System/TransformSystem.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <filesystem>
#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace MEngine
{
struct WindowConfig
{
    int32_t width = 1280;
    int32_t height = 720;
    std::string title = "MEngine Editor";
    bool fullscreen = false;
    bool resizable = true;
    bool vsync = true;
    std::string fontPath = "Assets/Fonts/NotoSans-Medium.ttf";
    float fontSize = 16.0f;
};
class Editor
{
  private:
    std::shared_ptr<entt::registry> mRegistry;
    std::shared_ptr<entt::registry> mAssetRegistry;
    std::vector<std::shared_ptr<ISystem>> mSystems;
    std::shared_ptr<ResourceManager> mResourceManager;

  private:
    GLFWwindow *mWindow;
    ImGuiIO *mImGuiIO;
    std::string mWindowTitle;
    uint32_t mWindowWidth;
    uint32_t mWindowHeight;

    WindowConfig mWindowConfig;
    bool mIsRunning = false;
    bool mIsFocused = false;
    bool mIsMinimized = false;
    bool mIsMaximized = false;
    bool mIsFullscreen = false;
    std::filesystem::path mDefaultFontPath = std::filesystem::current_path() / "Assets/Fonts/NotoSans-Medium.ttf";
    std::filesystem::path mUIResourcesPath = std::filesystem::current_path() / "Assets" / "UI";
    std::filesystem::path mAssetsPath = std::filesystem::current_path() / "Project";

  private:
    // UI
    ImGuiID mDockSpaceID;
    ImFont *mDefaultFont = nullptr;
    entt::entity mSelectedEntity = entt::null;
    entt::entity mHoveredEntity = entt::null;
    std::shared_ptr<entt::registry> mCurrentRegistry;
    std::filesystem::path mCurrentPath = mAssetsPath;
    uint32_t mAssetIconSize = 64;
    std::unordered_map<AssetType, ImTextureID> mAssetIcons;

  public:
    Editor();
    ~Editor();

    void Init();
    void InitWindow();
    void InitOpenGL();
    void InitImGui();
    void InitSystems();
    void Update(float deltaTime);
    void Shutdown();

    void EditorUI();
    void RenderViewportPanel();
    void RenderHierarchyPanel();
    void RenderInspectorPanel();
    void RenderAssetPanel();
    void LoadUIResources();
    void CreateAssetsForRaw(const std::filesystem::path &path);
    void LoadAssets(const std::filesystem::path &path);
    void AssetComponentUI();
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::WindowConfig>
{
    static void to_json(json &j, const MEngine::WindowConfig &config)
    {
        j["Window"]["Width"] = config.width;
        j["Window"]["Height"] = config.height;
        j["Window"]["Title"] = config.title;
        j["Window"]["Fullscreen"] = config.fullscreen;
        j["Window"]["Resizable"] = config.resizable;
        j["Window"]["Vsync"] = config.vsync;
        j["Window"]["FontPath"] = config.fontPath;
        j["Window"]["FontSize"] = config.fontSize;
    }

    static void from_json(const json &j, MEngine::WindowConfig &config)
    {
        config.width = j["Window"]["Width"].get<int32_t>();
        config.height = j["Window"]["Height"].get<int32_t>();
        config.title = j["Window"]["Title"].get<std::string>();
        config.fullscreen = j["Window"]["Fullscreen"].get<bool>();
        config.resizable = j["Window"]["Resizable"].get<bool>();
        config.vsync = j["Window"]["Vsync"].get<bool>();
        config.fontPath = j["Window"]["FontPath"].get<std::string>();
        config.fontSize = j["Window"]["FontSize"].get<int32_t>();
    }
};

} // namespace nlohmann