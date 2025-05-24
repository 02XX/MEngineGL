#pragma once
#include "AssetDatabase.hpp"
#include "Component/Reflection.hpp"
#include "System/RenderSystem.hpp"
#include "UUID.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>
#include <filesystem>
#include <format>
#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <ImGuizmo.h>

namespace MEngine
{
using namespace Editor;
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
struct Resolution
{
    int width = 1280;
    int height = 720;
    std::string ToString()
    {
        std::string resolution = std::format("{}x{}", width, height);
        return resolution;
    }
    bool operator==(const Resolution &other) const
    {
        return width == other.width && height == other.height;
    }
};
class MEngineEditor
{
  private:
    std::shared_ptr<entt::registry> mRegistry;
    std::vector<std::shared_ptr<ISystem>> mSystems;
    std::shared_ptr<RenderSystem> mRenderSystem;

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
    std::filesystem::path mAssetsPath = std::filesystem::current_path() / "Assets";
    std::filesystem::path mProjectPath = std::filesystem::current_path() / "Project";

    std::vector<Resolution> mResolutions = {{100, 100},   {800, 600},   {1280, 720}, {1920, 1080},
                                            {2560, 1440}, {3840, 2160}, {5120, 2880}};
    Resolution mCurrentResolution = {1280, 720};

  private:
    // UI
    ImGuiID mDockSpaceID;
    ImFont *mDefaultFont = nullptr;
    entt::entity mSelectedEntity = entt::null;
    entt::entity mHoveredEntity = entt::null;
    std::filesystem::path mCurrentPath = mProjectPath;
    uint32_t mAssetIconSize = 64;
    std::unordered_map<std::type_index, ImTextureID> mAssetIcons;
    float mGizmoWidth = 10.f;
    float mGizmoHeight = 10.f;
    ImGuizmo::OPERATION mGuizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE mGuizmoMode = ImGuizmo::LOCAL;
    entt::entity mEditorCamera;
    std::shared_ptr<AssetMeta> mSelectedAsset;
    std::shared_ptr<AssetMeta> mHoveredAsset;

  public:
    MEngineEditor();
    ~MEngineEditor();

    void Init();
    void InitWindow();
    void InitOpenGL();
    void InitImGui();
    void InitSystems();
    void Update(float deltaTime);
    void Shutdown();

    void EditorUI();
    void EditorCamera();
    void RenderToolbarPanel();
    void RenderViewportPanel();
    void RenderHierarchyPanel();
    void RenderHierarchyItem(entt::entity entity);
    void RenderInspectorPanel();
    void DeleteAsset(entt::entity entity);
    void RenderAssetPanel();
    void LoadUIResources();
    void GetAssetFromModel(const UUID &modelID, std::shared_ptr<entt::registry> registry);
    void CreateAssetsForRaw(const std::filesystem::path &path);
    void LoadAssets(const std::filesystem::path &path);
    template <typename T> void InspectorUI(T &object)
    {
        auto metaAny = entt::forward_as_meta(object);
        InspectorObject(metaAny, entt::resolve<T>());
    }
    void InspectorObject(entt::meta_any &object, entt::meta_type metaType)
    {
        auto *info = static_cast<Info *>(metaType.custom());
        if (info == nullptr)
            return;
        if (!info->Serializable)
            return;
        auto objectName = info->DisplayName;
        auto headerName = std::format("{}##{}", objectName.data(), object.type().id());
        bool modified = false;
        if (ImGui::CollapsingHeader(headerName.data(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (auto &&[id, base] : metaType.base())
            {
                InspectorObject(object, base);
            }
            for (auto &&[id, field] : metaType.data())
            {
                auto fieldCustom = static_cast<Info *>(field.custom());
                if (!fieldCustom->Serializable)
                    continue;
                const char *fieldDisplayName = fieldCustom->DisplayName.data();
                std::string label = std::format("##{}_{}", objectName.data(), fieldDisplayName);
                auto fieldType = field.type();
                entt::meta_any fieldValue = field.get(object);
                bool isEditable = false;
                ImGui::Columns(2, "##fields", false);
                const int labelWidth = 100;
                ImGui::SetColumnWidth(0, labelWidth);
                if (!fieldCustom->Editable)
                    ImGui::BeginDisabled();
                if (fieldType == entt::resolve<std::string>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<std::string>();
                    if (ImGui::InputText(label.c_str(), value.data(), value.capacity() + 1))
                    {
                        field.set(object, value);
                        modified = true;
                    }
                }
                else if (fieldType == entt::resolve<std::filesystem::path>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<std::filesystem::path>();
                    auto pathStr = value.string();
                    if (ImGui::InputText(label.c_str(), pathStr.data(), pathStr.capacity() + 1))
                    {
                        field.set(object, std::filesystem::path(pathStr));
                        modified = true;
                    }
                }
                else if (fieldType == entt::resolve<bool>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<bool>();
                    if (ImGui::Checkbox(label.c_str(), &value))
                    {
                        field.set(object, value);
                        modified = true;
                    }
                }
                else if (fieldType == entt::resolve<int>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<int>();
                    if (ImGui::InputInt(label.c_str(), &value))
                    {
                        field.set(object, std::clamp(value, 0, 100));
                        modified = true;
                    }
                }
                else if (fieldType == entt::resolve<float>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<float>();
                    if (ImGui::InputFloat(label.c_str(), &value))
                    {
                        field.set(object, std::clamp(value, 0.0f, 1.0f));
                        modified = true;
                    }
                }
                else if (fieldType == entt::resolve<AssetType>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<AssetType>();
                    auto current = magic_enum::enum_name(value);
                    if (ImGui::BeginCombo(label.c_str(), current.data()))
                    {
                        for (auto [enumValue, enumName] : magic_enum::enum_entries<AssetType>())
                        {
                            bool is_selected = (enumValue == value);
                            if (ImGui::Selectable(enumName.data(), is_selected))
                            {
                                field.set(object, enumValue);
                                modified = true;
                            }
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
                // else if (fieldType == entt::resolve<Material>())
                // {
                //     ImGui::Text("%s: ", fieldDisplayName);
                //     ImGui::NextColumn();
                //     auto value = fieldValue.cast<AssetType>();
                //     auto current = magic_enum::enum_name(value);
                //     if (ImGui::BeginCombo(label.c_str(), current.data()))
                //     {
                //         for (auto [enumValue, enumName] : magic_enum::enum_entries<AssetType>())
                //         {
                //             bool is_selected = (enumValue == value);
                //             if (ImGui::Selectable(enumName.data(), is_selected))
                //             {
                //                 field.set(object, enumValue);
                //                 modified = true;
                //             }
                //             if (is_selected)
                //             {
                //                 ImGui::SetItemDefaultFocus();
                //             }
                //         }
                //         ImGui::EndCombo();
                //     }
                // }
                else if (fieldType == entt::resolve<UUID>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<UUID>();
                    auto uuidStr = value.ToString();
                    if (ImGui::InputText(label.c_str(), uuidStr.data(), uuidStr.capacity() + 1))
                    {
                        field.set(object, UUID(uuidStr));
                        modified = true;
                    }
                    // if (!fieldCustom->Editable)
                    //     ImGui::EndDisabled();
                    // ImGui::Columns(1);
                    // if (auto it = std::dynamic_pointer_cast<Texture2D>(entity))
                    // {
                    //     auto metaAny = entt::forward_as_meta(*it);
                    //     InspectorObject(metaAny, entt::resolve<Texture2D>(), dirty);
                    // }
                    // ImGui::Columns(2, "##fields", false);
                    // if (!fieldCustom->Editable)
                    //     ImGui::BeginDisabled();
                }
                else if (fieldType == entt::resolve<glm::vec2>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<glm::vec2>();
                    if (ImGui::DragFloat2(label.c_str(), &value[0], 0.1f))
                    {
                        field.set(object, value);
                        modified = true;
                    }
                }
                else if (fieldType == entt::resolve<glm::vec3>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<glm::vec3>();
                    if (ImGui::DragFloat3(label.c_str(), &value[0], 0.1f))
                    {
                        field.set(object, value);
                        modified = true;
                    }
                }
                else if (fieldType == entt::resolve<glm::vec4>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<glm::vec4>();
                    if (ImGui::InputFloat4(label.c_str(), &value[0]))
                    {
                        field.set(object, value);
                        modified = true;
                    }
                }
                else if (fieldType == entt::resolve<glm::quat>())
                {
                    ImGui::Text("%s: ", fieldDisplayName);
                    ImGui::NextColumn();
                    auto value = fieldValue.cast<glm::quat>();
                    glm::vec3 euler = glm::degrees(glm::eulerAngles(value));
                    if (ImGui::DragFloat3(label.c_str(), &euler[0], 0.1f))
                    {
                        value = glm::quat(glm::radians(euler));
                        field.set(object, value);
                        modified = true;
                    }
                }
                // else if (fieldType == entt::resolve<Importer>())
                // {
                //     InspectorObject(object, entt::resolve<Importer>());
                // }
                else
                {
                    // LogDebug("{}: {}, editable: {}", fieldName, typeid(ValueType).name(), isEditable);
                }
                if (!fieldCustom->Editable)
                    ImGui::EndDisabled();
                ImGui::Columns(1);
            };
        }
        if (modified)
        {
            // if (metaType == entt::resolve<Texture2D>())
            // {
            //     auto entity = object.try_cast<Texture2D>();
            //     auto texture = mResourceManager->GetAsset<Texture2D>(entity->ID);
            //     mResourceManager->UpdateAsset<Texture2D>(texture->ID, texture);
            // }
        }
    }
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