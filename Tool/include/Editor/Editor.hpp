#pragma once
#include "IConfigure.hpp"
#include "Logger.hpp"
#include "ResourceManager.hpp"
#include "SceneManager.hpp"
#include "System/TransformSystem.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <entt/entt.hpp>
#include <filesystem>
#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <memory>
#include <string>
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
    float fontSize = 16;
};
class Editor
{
  private:
    std::shared_ptr<entt::registry> mRegistry;
    std::vector<std::shared_ptr<ISystem>> mSystems;

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

  private:
    // UI
    ImGuiID mDockSpaceID;
    ImFont *mDefaultFont = nullptr;

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