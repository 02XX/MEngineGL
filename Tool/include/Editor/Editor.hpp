#pragma once
#include "IConfigure.hpp"
#include "Logger.hpp"
#include "ResourceManager.hpp"
#include "SceneManager.hpp"
#include "System/TransformSystem.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <entt/entt.hpp>
#include <memory>
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
};
class Editor
{
  private:
    std::shared_ptr<entt::registry> mRegistry;
    std::vector<std::shared_ptr<ISystem>> mSystems;

  private:
    GLFWwindow *mWindow;

    std::string mWindowTitle;
    uint32_t mWindowWidth;
    uint32_t mWindowHeight;

    WindowConfig mWindowConfig;
    bool mIsRunning = false;
    bool mIsFocused = false;
    bool mIsMinimized = false;
    bool mIsMaximized = false;
    bool mIsFullscreen = false;

  public:
    Editor();
    ~Editor();

    void Init();
    void InitWindow();
    void InitOpenGL();
    void InitSystems();
    void Update(float deltaTime);
    void Shutdown();
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
    }

    static void from_json(const json &j, MEngine::WindowConfig &config)
    {
        config.width = j["Window"]["Width"].get<int32_t>();
        config.height = j["Window"]["Height"].get<int32_t>();
        config.title = j["Window"]["Title"].get<std::string>();
        config.fullscreen = j["Window"]["Fullscreen"].get<bool>();
        config.resizable = j["Window"]["Resizable"].get<bool>();
        config.vsync = j["Window"]["Vsync"].get<bool>();
    }
};

} // namespace nlohmann