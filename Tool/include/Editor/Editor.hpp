#pragma once
#include "Component/AssestComponent.hpp"
#include "Component/Reflection.hpp"
#include "Component/TextureComponent.hpp"
#include "Entity/IEntity.hpp"
#include "Entity/Texture2D.hpp"
#include "IConfigure.hpp"
#include "Logger.hpp"
#include "Repository/Repository.hpp"
#include "ResourceManager.hpp"
#include "SceneManager.hpp"
#include "System/TransformSystem.hpp"
#include <GLFW/glfw3.h>
#include <concepts>
#include <cstdint>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <entt/meta/resolve.hpp>
#include <filesystem>
#include <format>
#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <refl.hpp>
#include <string>
#include <type_traits>
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
    template <typename TComponent>
        requires std::derived_from<TComponent, Component>
    void ComponentUI(TComponent &component)
    {
        InspectorUI(component, component.dirty);
    }
    template <typename T> void InspectorUIOld(T &object, bool &dirty)
    {
        const refl::const_string componentName = refl::reflect<T>().name;
        if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Columns(1);
            refl::util::for_each(refl::reflect<T>().members, [&](auto member, std::size_t index) {
                const char *fieldName = refl::descriptor::get_display_name(member).c_str();
                std::string label = std::format("##{}_{}", componentName.c_str(), fieldName);
                auto &value = member(object);
                bool isEditable = refl::descriptor::has_attribute<Editable>(member);
                ImGui::Columns(2, "##fields", false);
                const int labelWidth = 100;
                ImGui::SetColumnWidth(0, labelWidth);
                ImGui::Text("%s: ", fieldName);
                ImGui::NextColumn();
                using ValueType = std::remove_reference_t<decltype(value)>;

                if constexpr (std::is_same_v<ValueType, std::string>)
                {
                    if (!isEditable)
                        ImGui::BeginDisabled();
                    if (ImGui::InputText(label.c_str(), value.data(), value.capacity() + 1))
                    {
                        value = std::string(value);
                        dirty = true;
                    }
                    if (!isEditable)
                        ImGui::EndDisabled();
                }
                else if constexpr (std::is_same_v<ValueType, std::filesystem::path>)
                {
                    if (!isEditable)
                        ImGui::BeginDisabled();
                    auto pathStr = value.string();
                    if (ImGui::InputText(label.c_str(), pathStr.data(), pathStr.capacity() + 1))
                    {
                        value = std::filesystem::path(value);
                        dirty = true;
                    }
                    if (!isEditable)
                        ImGui::EndDisabled();
                }
                else if constexpr (std::is_same_v<ValueType, bool>)
                {
                    if (!isEditable)
                        ImGui::BeginDisabled();
                    if (ImGui::Checkbox(label.c_str(), &value))
                    {
                        dirty = true;
                    }
                    if (!isEditable)
                        ImGui::EndDisabled();
                }
                else if constexpr (std::is_same_v<ValueType, int>)
                {
                    if (!isEditable)
                        ImGui::BeginDisabled();
                    if (ImGui::InputInt(label.c_str(), &value))
                    {
                        value = std::clamp(value, 0, 100);
                        dirty = true;
                        if constexpr (std::is_base_of_v<IEntity, T>)
                        {
                            auto entity = mResourceManager->GetAsset<T>(object.GetID());
                            mResourceManager->UpdateAsset(object.GetID(), entity);
                        }
                    }
                    if (!isEditable)
                        ImGui::EndDisabled();
                }
                else if constexpr (std::is_same_v<ValueType, float>)
                {
                    if (!isEditable)
                        ImGui::BeginDisabled();
                    if (ImGui::InputFloat(label.c_str(), &value))
                    {
                        value = std::clamp(value, 0.0f, 1.0f);
                        dirty = true;
                    }
                    if (!isEditable)
                        ImGui::EndDisabled();
                }
                else if constexpr (std::is_enum_v<ValueType>)
                {
                    if (!isEditable)
                        ImGui::BeginDisabled();
                    auto current = magic_enum::enum_name(value);
                    if (ImGui::BeginCombo(label.c_str(), current.data()))
                    {
                        for (auto [enumValue, enumName] : magic_enum::enum_entries<ValueType>())
                        {
                            bool is_selected = (enumValue == value);
                            if (ImGui::Selectable(enumName.data(), is_selected))
                            {
                                value = enumValue;
                                dirty = true;
                            }
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    if (!isEditable)
                        ImGui::EndDisabled();
                }
                else if constexpr (requires { typename ValueType::element_type; })
                {
                    using RawType = typename ValueType::element_type;
                    if constexpr (std::is_base_of_v<IEntity, RawType>)
                    {
                        ImGui::Columns(1);
                        if (typeid(*value) == typeid(Texture2D))
                        {
                            auto texture2D = std::dynamic_pointer_cast<Texture2D>(value);
                            InspectorUI<Texture2D>(*texture2D, dirty);
                        }
                        ImGui::Columns(2, "##fields", false);
                        ImGui::SetColumnWidth(0, labelWidth);
                        ImGui::NextColumn();
                    }
                }
                else
                {
                    // LogDebug("{}: {}, editable: {}", fieldName, typeid(ValueType).name(), isEditable);
                }
                ImGui::Columns(1);
            });
        }
    }
    template <typename T> void InspectorUI(T &object, bool &dirty)
    {
        const refl::const_string componentName = refl::reflect<T>().name;
        if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Columns(1);
            auto metaType = entt::resolve<T>();
            for (auto &&[id, field] : metaType.data())
            {
                const char *fieldDisplayName = id;
                std::string label = std::format("##{}_{}", componentName.c_str(), fieldDisplayName);
                auto fieldType = field.type();
                auto fieldValue = field.get(object);
                bool isEditable = false;
                ImGui::Columns(2, "##fields", false);
                const int labelWidth = 100;
                ImGui::SetColumnWidth(0, labelWidth);
                ImGui::Text("%s: ", fieldDisplayName);
                ImGui::NextColumn();
                if (!isEditable)
                    ImGui::BeginDisabled();
                if (fieldType == entt::resolve<std::string>())
                {
                    if (!isEditable)
                        ImGui::BeginDisabled();
                    if (ImGui::InputText(label.c_str(), fieldValue.data(), fieldValue.capacity() + 1))
                    {
                        fieldValue.set(id, fieldValue);
                        dirty = true;
                    }
                }
                else if (fieldType == entt::resolve<std::filesystem::path>())
                {
                    auto pathStr = fieldValue.string();
                    if (ImGui::InputText(label.c_str(), pathStr.data(), pathStr.capacity() + 1))
                    {
                        fieldValue.set(id, std::filesystem::path(pathStr));
                        dirty = true;
                    }
                }
                else if (fieldType == entt::resolve<bool>())
                {
                    if (!isEditable)
                        ImGui::BeginDisabled();
                    if (ImGui::Checkbox(label.c_str(), &fieldValue))
                    {
                        fieldValue.set(id, fieldValue);
                        dirty = true;
                    }
                    if (!isEditable)
                        ImGui::EndDisabled();
                }
                else if (fieldType == entt::resolve<int>())
                {
                    if (!isEditable)
                        ImGui::BeginDisabled();
                    if (ImGui::InputInt(label.c_str(), &fieldValue))
                    {
                        fieldValue.set(id, std::clamp(fieldValue, 0, 100));
                        dirty = true;
                        if constexpr (std::is_base_of_v<IEntity, T>)
                        {
                            auto entity = mResourceManager->GetAsset<T>(object.GetID());
                            mResourceManager->UpdateAsset(object.GetID(), entity);
                        }
                    }
                    if (!isEditable)
                        ImGui::EndDisabled();
                }
                else if (fieldType == entt::resolve<float>())
                {
                    if (ImGui::InputFloat(label.c_str(), &fieldValue))
                    {
                        fieldValue.set(id, std::clamp(fieldValue, 0.0f, 1.0f));
                        dirty = true;
                    }
                    if (!isEditable)
                        ImGui::EndDisabled();
                }
                // else if (fieldType.is_enum())
                // {
                //     if (!isEditable)
                //         ImGui::BeginDisabled();
                //     auto current = magic_enum::enum_name(fieldValue);
                //     if (ImGui::BeginCombo(label.c_str(), current.data()))
                //     {
                //         for (auto [enumValue, enumName] : magic_enum::enum_entries<ValueType>())
                //         {
                //             bool is_selected = (enumValue == value);
                //             if (ImGui::Selectable(enumName.data(), is_selected))
                //             {
                //                 value = enumValue;
                //                 dirty = true;
                //             }
                //             if (is_selected)
                //             {
                //                 ImGui::SetItemDefaultFocus();
                //             }
                //         }
                //         ImGui::EndCombo();
                //     }
                //     if (!isEditable)
                //         ImGui::EndDisabled();
                // }
                // else if constexpr (requires { typename ValueType::element_type; })
                // {
                //     using RawType = typename ValueType::element_type;
                //     if constexpr (std::is_base_of_v<IEntity, RawType>)
                //     {
                //         ImGui::Columns(1);
                //         if (typeid(*value) == typeid(Texture2D))
                //         {
                //             auto texture2D = std::dynamic_pointer_cast<Texture2D>(value);
                //             InspectorUI<Texture2D>(*texture2D, dirty);
                //         }
                //         ImGui::Columns(2, "##fields", false);
                //         ImGui::SetColumnWidth(0, labelWidth);
                //         ImGui::NextColumn();
                //     }
                // }
                else
                {
                    // LogDebug("{}: {}, editable: {}", fieldName, typeid(ValueType).name(), isEditable);
                }
                if (!isEditable)
                    ImGui::EndDisabled();
            };
            ImGui::Columns(1);
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