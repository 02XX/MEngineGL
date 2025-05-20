#include "Editor/Editor.hpp"
#include "Component/AssestComponent.hpp"
#include "Component/TextureComponent.hpp"
#include "Component/TransformComponent.hpp"
#include "Entity/IMaterial.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Entity/Pipeline.hpp"
#include "Entity/Texture2D.hpp"
#include "IConfigure.hpp"
#include "Logger.hpp"
#include <GLFW/glfw3.h>
#include <boost/di.hpp>
#include <cstddef>
#include <filesystem>
#include <imgui.h>
#include <imgui_freetype.h>
#include <memory>
#include <typeindex>
namespace MEngine
{
auto injector = boost::di::make_injector(boost::di::bind<IConfigure>().to<Configure>().in(boost::di::unique));

void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                              const GLchar *message, const void *userParam)
{
    // 忽略通知级别的信息
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    // 解析消息来源
    const char *_source;
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        _source = "Other";
        break;
    default:
        _source = "Unknown";
    }

    // 解析消息类型
    const char *_type;
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        _type = "Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "Deprecated Behavior";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "Undefined Behavior";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        _type = "Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        _type = "Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        _type = "Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        _type = "Other";
        break;
    default:
        _type = "Unknown";
    }

    // 根据严重程度选择适当的日志级别
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        LogError("[OpenGL] [{}] [{}] (ID: {}): {}", _source, _type, id, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        LogWarn("[OpenGL] [{}] [{}] (ID: {}): {}", _source, _type, id, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        LogDebug("[OpenGL] [{}] [{}] (ID: {}): {}", _source, _type, id, message);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
    default:
        LogTrace("[OpenGL] [{}] [{}] (ID: {}): {}", _source, _type, id, message);
        break;
    }
}
Editor::Editor()
{
    mRegistry = std::make_shared<entt::registry>();
    mResourceManager = injector.create<std::shared_ptr<ResourceManager>>();
    LogInfo("Editor initialized");
}
Editor::~Editor()
{
    LogInfo("Editor destroyed");
}
void Editor::Init()
{
    RegisterMeta();
    InitWindow();
    InitOpenGL();
    InitSystems();
    InitImGui();
    LoadUIResources();
    LoadAssets(mAssetsPath);
    mIsRunning = true;
}
void Editor::InitWindow()
{
    // 读取配置文件
    mWindowConfig = injector.create<std::shared_ptr<IConfigure>>()->GetJson().get<WindowConfig>();

    if (!glfwInit())
    {
        LogError("Failed to initialize GLFW");
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, mWindowConfig.resizable ? GLFW_TRUE : GLFW_FALSE);
    if (mWindowConfig.fullscreen)
    {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        mWindowConfig.width = mode->width;
        mWindowConfig.height = mode->height;
        mWindow = glfwCreateWindow(mWindowConfig.width, mWindowConfig.height, mWindowConfig.title.c_str(),
                                   glfwGetPrimaryMonitor(), nullptr);
        glfwWindowHint(GLFW_DECORATED, mWindowConfig.fullscreen ? GLFW_FALSE : GLFW_TRUE);
        mIsFullscreen = true;
    }
    else
    {
        mWindow =
            glfwCreateWindow(mWindowConfig.width, mWindowConfig.height, mWindowConfig.title.c_str(), nullptr, nullptr);
    }
    if (!mWindow)
    {
        LogError("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
    if (mWindowConfig.vsync)
    {
        glfwSwapInterval(1); // 启用垂直同步
        LogInfo("VSync enabled");
    }
    else
    {
        glfwSwapInterval(0); // 禁用垂直同步
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwMakeContextCurrent(mWindow);
}
void Editor::InitOpenGL()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LogError("Failed to initialize GLAD");
        return;
    }
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // 确保回调在错误发生的线程调用
    glDebugMessageCallback(DebugCallback, nullptr);

    LogInfo("OpenGL version: {}.{}", GLVersion.major, GLVersion.minor);
    LogInfo("GLSL version: {}", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    LogInfo("Vendor: {}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
    LogInfo("Renderer: {}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
}
void Editor::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    mImGuiIO = &ImGui::GetIO();
    mImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    std::filesystem::path fontPath = std::filesystem::current_path() / mWindowConfig.fontPath;
    mImGuiIO->Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
    mDefaultFont = mImGuiIO->Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 24.0f, nullptr,
                                                       mImGuiIO->Fonts->GetGlyphRangesChineseSimplifiedCommon());
    mImGuiIO->FontDefault = mDefaultFont;
}
void Editor::InitSystems()
{
    mSystems.push_back(std::make_shared<TransformSystem>(mRegistry));

    for (auto &system : mSystems)
    {
        system->Init();
    }
}
void Editor::Update(float deltaTime)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        if (mIsRunning)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            EditorUI();

            // Render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            for (auto &system : mSystems)
            {
                system->Update(deltaTime);
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(mWindow);
        }
    }
}
void Editor::Shutdown()
{
    for (auto &system : mSystems)
    {
        system->Shutdown();
    }
    mIsRunning = false;
}

//=======================Editor UI=========================
void Editor::EditorUI()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    mDockSpaceID = ImGui::DockSpaceOverViewport();

    // 3. 首次运行时初始化布局
    static bool first_run = true;
    if (first_run)
    {
        first_run = false;
        ImGui::DockBuilderRemoveNode(mDockSpaceID);
        ImGui::DockBuilderAddNode(mDockSpaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(mDockSpaceID, viewport->WorkSize);
        // 1. 主区域拆分为底部（30%）和顶部（70%）
        ImGuiID dockBottomID, dockTopID;
        ImGui::DockBuilderSplitNode(mDockSpaceID, ImGuiDir_Down, 0.3, &dockBottomID, &dockTopID);
        // 2. 顶部区域拆分为左（30%）和剩余部分（70%）
        ImGuiID dockLeftID, remainingTop;
        ImGui::DockBuilderSplitNode(dockTopID, ImGuiDir_Left, 0.3, &dockLeftID, &remainingTop);
        // 3. 剩余部分（70%）拆分为中（60%）和右（40%）
        ImGuiID dockCenterID, dockRightID;
        ImGui::DockBuilderSplitNode(remainingTop, ImGuiDir_Right, 0.4, &dockRightID, &dockCenterID);
        // 4. 中部拆分上(20%)和下(80%)
        ImGuiID dockTopCenterID, dockBottomCenterID;
        ImGui::DockBuilderSplitNode(dockCenterID, ImGuiDir_Up, 0.15, &dockTopCenterID, &dockBottomCenterID);
        // 绑定窗口
        ImGui::DockBuilderDockWindow("Viewport", dockBottomCenterID); // 中间
        ImGui::DockBuilderDockWindow("Hierarchy", dockLeftID);        // 左侧
        ImGui::DockBuilderDockWindow("Inspector", dockRightID);       // 右侧
        ImGui::DockBuilderDockWindow("Assets", dockBottomID);         // 底部
        ImGui::DockBuilderDockWindow("Toolbar", dockTopCenterID);     // 顶部
        ImGui::DockBuilderFinish(mDockSpaceID);
    }
    RenderViewportPanel();
    RenderHierarchyPanel();
    RenderInspectorPanel();
    RenderAssetPanel();
}
void Editor::RenderViewportPanel()
{
    ImGui::Begin("Viewport");
    ImGui::Text("Viewport");
    ImGui::End();
}
void Editor::RenderHierarchyPanel()
{
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_None);
    auto view = mRegistry->view<TransformComponent>();
    std::vector<entt::entity> rootEntities;
    for (auto entity : view)
    {
        auto &transform = view.get<TransformComponent>(entity);
        if (transform.parent == entt::null)
        {
            rootEntities.push_back(entity);
        }
    }
    if (ImGui::BeginPopupContextWindow("HierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Create Entity"))
        {
            auto entity = mRegistry->create();
            auto &transform = mRegistry->emplace<TransformComponent>(entity);
        }

        ImGui::EndPopup();
    }
    for (auto entity : rootEntities)
    {
        RenderHierarchyItem(entity);
    }
    ImGui::End();
}
void Editor::RenderHierarchyItem(entt::entity entity)
{
    auto &transform = mRegistry->get<TransformComponent>(entity);
    std::string name = transform.name;
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (transform.children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }
    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)entity, flags, "%s", name.data());
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        mSelectedEntity = entity;
    }
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("ENTITY_DRAG_DROP", &entity, sizeof(entt::entity));
        ImGui::Text("Move %s", name.data());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_DRAG_DROP"))
        {
            IM_ASSERT(payload->DataSize == sizeof(entt::entity));
            entt::entity draggedEntity = *(const entt::entity *)payload->Data;

            // 重新设置父子关系
            auto &draggedTransform = mRegistry->get<TransformComponent>(draggedEntity);
            auto &targetTransform = mRegistry->get<TransformComponent>(entity);
            if (draggedTransform.parent != entity)
            {
                // 移除拖拽实体的父节点
                if (draggedTransform.parent != entt::null)
                {
                    auto &parentTransform = mRegistry->get<TransformComponent>(draggedTransform.parent);
                    auto it =
                        std::find(parentTransform.children.begin(), parentTransform.children.end(), draggedEntity);
                    if (it != parentTransform.children.end())
                    {
                        parentTransform.children.erase(it);
                    }
                }
                // 设置新的父节点
                draggedTransform.parent = entity;
                targetTransform.children.push_back(draggedEntity);
            }
        }
        ImGui::EndDragDropTarget();
    }
    // 添加空白区域的拖拽目标
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    ImRect windowRect(window->InnerRect.Min, window->InnerRect.Max);

    // 创建一个覆盖整个窗口的透明拖拽目标
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

    if (ImGui::BeginDragDropTargetCustom(windowRect, ImGui::GetID("EmptySpaceDragDropTarget")))
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_DRAG_DROP"))
        {
            IM_ASSERT(payload->DataSize == sizeof(entt::entity));
            entt::entity draggedEntity = *(const entt::entity *)payload->Data;

            auto &draggedTransform = mRegistry->get<TransformComponent>(draggedEntity);

            // 如果当前有父节点，则移除父节点关系
            if (draggedTransform.parent != entt::null)
            {
                auto &parentTransform = mRegistry->get<TransformComponent>(draggedTransform.parent);
                auto it = std::find(parentTransform.children.begin(), parentTransform.children.end(), draggedEntity);
                if (it != parentTransform.children.end())
                {
                    parentTransform.children.erase(it);
                }
                draggedTransform.parent = entt::null;
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::PopStyleColor(3);
    // 递归绘制子节点
    if (opened)
    {
        for (auto child : transform.children)
        {
            RenderHierarchyItem(child);
        }
        ImGui::TreePop();
    }
}
void Editor::RenderInspectorPanel()
{
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None);
    if (mSelectedEntity != entt::null)
    {
        auto entityLabel = std::format("Entity {}", static_cast<uint32_t>(mSelectedEntity));
        ImGui::Text("%s", entityLabel.c_str());
        if (mRegistry->any_of<AssetsComponent>(mSelectedEntity))
        {
            auto &assetComponent = mRegistry->get<AssetsComponent>(mSelectedEntity);
            ComponentUI(assetComponent);
        }
        if (mRegistry->any_of<TextureComponent>(mSelectedEntity))
        {
            auto &textureComponent = mRegistry->get<TextureComponent>(mSelectedEntity);
            ComponentUI(textureComponent);
        }
        if (mRegistry->any_of<TransformComponent>(mSelectedEntity))
        {
            auto &transformComponent = mRegistry->get<TransformComponent>(mSelectedEntity);
            ComponentUI(transformComponent);
        }
    }
    ImGui::End();
}

void Editor::RenderAssetPanel()
{
    ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_None);
    if (ImGui::Button("<-"))
    {
        if (mCurrentPath != mAssetsPath)
        {
            mCurrentPath = mCurrentPath.parent_path();
        }
    }
    ImGui::SameLine();
    ImGui::Text("%s", mCurrentPath.string().c_str());
    ImGui::Separator();
    auto view = mRegistry->view<AssetsComponent>();
    int columns = std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / (mAssetIconSize + 10)));
    ImGui::Columns(columns, "AssetColumns", false); // false = 不显示边框
    for (auto entity : view)
    {
        auto &assetComponent = view.get<AssetsComponent>(entity);
        if (assetComponent.path.parent_path() == mCurrentPath)
        {
            ImGui::PushID(entt::to_integral(entity));
            ImTextureID textureID = 0;
            if (mAssetIcons.find(assetComponent.type) != mAssetIcons.end())
            {
                textureID = mAssetIcons[assetComponent.type];
            }
            if (ImGui::Selectable("##btn", mSelectedEntity == entity,
                                  ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick,
                                  ImVec2(mAssetIconSize + 10, mAssetIconSize + 20)))
            {
                mSelectedEntity = entity;
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    if (assetComponent.type == AssetType::Folder)
                    {
                        mCurrentPath = assetComponent.path;
                    }
                }
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
            {
                mHoveredEntity = entity;
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                    mSelectedEntity = entity;
                if (ImGui::IsMouseDragging(0))
                {
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                    {
                        // 设置拖拽数据
                        ImGui::SetDragDropPayload("ASSET_ITEM", &entity, sizeof(entity));
                        // 显示拖拽预览（图标）
                        ImGui::Image(textureID, ImVec2(0, 1), ImVec2(1, 0));
                        // 可选：添加文字说明
                        ImGui::Text("拖动 %s", assetComponent.name.c_str());
                        ImGui::EndDragDropSource();
                    }
                }
            }
            ImVec2 ContainerMinPos = ImGui::GetItemRectMin();
            ImVec2 ContainerMaxPos = ImGui::GetItemRectMax();
            ImVec2 ContainerSize = ImGui::GetItemRectSize();
            ImVec2 iconPos = ImVec2(ContainerMinPos.x, ContainerMinPos.y);
            ImGui::SetCursorScreenPos(iconPos);
            ImGui::Image(textureID, ImVec2(mAssetIconSize, mAssetIconSize), ImVec2(0, 1), ImVec2(1, 0));
            // 绘制文本
            ImVec2 textPos =
                ImVec2(ContainerMinPos.x + (ContainerSize.x - mAssetIconSize) / 2, ContainerMinPos.y + mAssetIconSize);
            ImGui::SetCursorScreenPos(textPos);
            ImGui::Text("%s", assetComponent.name.c_str());
            ImGui::PopID();
            ImGui::NextColumn(); // 移动到下一列
        }
    }
    ImGui::Columns(1); // 恢复单列模式
    ImGui::End();
}
void Editor::LoadUIResources()
{
    auto foldTexture =
        std::dynamic_pointer_cast<Texture2D>(mResourceManager->CreateAsset(mUIResourcesPath / "folder.png"));
    auto fileTexture =
        std::dynamic_pointer_cast<Texture2D>(mResourceManager->CreateAsset(mUIResourcesPath / "file.png"));
    mAssetIcons[AssetType::Folder] = foldTexture->GetTextureID();
    mAssetIcons[AssetType::File] = fileTexture->GetTextureID();
    LogInfo("Loaded UI resources");
}
void Editor::LoadAssets(const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error("Project path does not exist: " + path.string());
    }
    static const std::unordered_map<std::type_index, AssetType> typeToAssetType = {
        {typeid(Pipeline), AssetType::Shader},
        {typeid(PBRMaterial), AssetType::PBRMaterial},
        {typeid(Texture2D), AssetType::Texture2D}};
    auto directory = std::filesystem::directory_iterator(path);
    for (auto &entry : directory)
    {
        AssetsComponent assetComponent;
        assetComponent.path = entry.path();
        assetComponent.name = entry.path().filename().string();
        assetComponent.type = AssetType::File;
        if (entry.is_directory())
        {
            assetComponent.type = AssetType::Folder;
            auto entity = mRegistry->create();
            mRegistry->emplace<AssetsComponent>(entity, assetComponent);
            LoadAssets(entry.path());
        }
        else if (entry.is_regular_file())
        {
            auto extension = entry.path().extension().string();
            std::type_index type = typeid(void);
            if (mResourceManager->IsAsset(entry.path()))
            {
                type = mResourceManager->GetAssetTypeFromExtension(extension);
                auto asset = mResourceManager->LoadAsset(entry.path());
                auto it = typeToAssetType.find(type);
                if (it != typeToAssetType.end())
                {
                    assetComponent.type = it->second;
                }
                auto entity = mRegistry->create();
                mRegistry->emplace<AssetsComponent>(entity, assetComponent);
                if (type == typeid(Texture2D))
                {
                    auto &textureComponent = mRegistry->emplace<TextureComponent>(entity);
                    textureComponent.textureID = asset->GetID();
                }
            }
            else
            {
                auto assetPath = entry.path();
                type = mResourceManager->GetRawTypeFromExtension(extension);
                assetPath.replace_extension(mResourceManager->GetAssetExtensionFromType(type));
                if (!std::filesystem::exists(assetPath))
                {
                    mResourceManager->CreateAsset(entry.path());
                    assetComponent.path = assetPath;
                    assetComponent.name = assetPath.filename().string();
                    auto it = typeToAssetType.find(type);
                    if (it != typeToAssetType.end())
                    {
                        assetComponent.type = it->second;
                    }
                    auto entity = mRegistry->create();
                    mRegistry->emplace<AssetsComponent>(entity, assetComponent);
                }
            }
        }
    }
}
} // namespace MEngine
