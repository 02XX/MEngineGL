#include "Editor/MEngineEditor.hpp"
#include "Asset/Asset.hpp"
#include "Asset/Folder.hpp"
#include "Asset/Mesh.hpp"
#include "Asset/Model.hpp"
#include "Asset/PBRMaterial.hpp"
#include "AssetDatabase.hpp"
#include "Component/TransformComponent.hpp"
#include "Configure.hpp"
#include "Logger.hpp"
#include "System/CameraSystem.hpp"
#include "System/RenderSystem.hpp"
#include "System/TransformSystem.hpp"
#include "UUID.hpp"
#include <boost/di.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <imgui.h>
#include <memory>

namespace MEngine
{
using namespace Editor;
namespace DI = boost::di;
auto injector = DI::make_injector(DI::bind<IConfigure>().to<Configure>().in(DI::unique),

                                  DI::bind<RenderSystem>().to<RenderSystem>().in(DI::singleton),
                                  DI::bind<TransformSystem>().to<TransformSystem>().in(DI::singleton),
                                  DI::bind<CameraSystem>().to<CameraSystem>().in(DI::singleton),
                                  DI::bind<entt::registry>().to<entt::registry>().in(DI::singleton));

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
MEngineEditor::MEngineEditor()
{
    mRegistry = injector.create<std::shared_ptr<entt::registry>>();
    mRenderSystem = injector.create<std::shared_ptr<RenderSystem>>();
    LogInfo("Editor initialized");
}
MEngineEditor::~MEngineEditor()
{
    LogInfo("Editor destroyed");
}
void MEngineEditor::Init()
{
    RegisterMeta();
    InitWindow();
    InitOpenGL();
    InitSystems();
    InitImGui();
    LoadUIResources();
    // camera
    auto camera = mRegistry->create();
    auto &cameraComponent = mRegistry->emplace<CameraComponent>(camera);
    auto &transformComponent = mRegistry->emplace<TransformComponent>(camera);
    cameraComponent.isEditorCamera = true;
    cameraComponent.aspectRatio = 16.0f / 9.0f;
    transformComponent.localPosition = glm::vec3(0.0f, 0.0f, 100.0f);
    transformComponent.name = "EditorCamera";
    mIsRunning = true;
}
void MEngineEditor::InitWindow()
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
void MEngineEditor::InitOpenGL()
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
void MEngineEditor::InitImGui()
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

    ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
    ImGuizmo::Enable(true);
}
void MEngineEditor::InitSystems()
{
    auto transformSystem = injector.create<std::shared_ptr<TransformSystem>>();
    auto cameraSystem = injector.create<std::shared_ptr<CameraSystem>>();
    auto renderSystem = injector.create<std::shared_ptr<RenderSystem>>();
    mSystems.push_back(transformSystem);
    mSystems.push_back(cameraSystem);
    mSystems.push_back(renderSystem);
    for (auto &system : mSystems)
    {
        system->Init();
    }
}
void MEngineEditor::Update(float deltaTime)
{
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        if (mIsRunning)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();
            EditorUI();

            // Render
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
void MEngineEditor::Shutdown()
{
    for (auto &system : mSystems)
    {
        system->Shutdown();
    }
    mIsRunning = false;
}

//=======================Editor UI=========================
void MEngineEditor::EditorUI()
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
        ImGui::DockBuilderSplitNode(dockCenterID, ImGuiDir_Up, 0.2, &dockTopCenterID, &dockBottomCenterID);
        // 绑定窗口
        ImGui::DockBuilderDockWindow("Viewport", dockBottomCenterID); // 中间
        ImGui::DockBuilderDockWindow("Hierarchy", dockLeftID);        // 左侧
        ImGui::DockBuilderDockWindow("Inspector", dockRightID);       // 右侧
        ImGui::DockBuilderDockWindow("Assets", dockBottomID);         // 底部
        ImGui::DockBuilderDockWindow("Toolbar", dockTopCenterID);     // 顶部
        ImGui::DockBuilderFinish(mDockSpaceID);
    }
    EditorCamera();
    RenderInspectorPanel();
    RenderAssetPanel();
    RenderViewportPanel();
    RenderToolbarPanel();
    RenderHierarchyPanel();
}
void MEngineEditor::EditorCamera()
{
    // 获取相机组件
    auto view = mRegistry->view<CameraComponent>();
    for (auto entity : view)
    {
        auto &cameraComponent = view.get<CameraComponent>(entity);
        if (cameraComponent.isEditorCamera)
        {
            mEditorCamera = entity;
            // if (mSelectedAsset != entt::null)
            // {
            //     auto &transformComponent = mRegistry->get<TransformComponent>(mSelectedAsset);
            //     cameraComponent.target = transformComponent.worldPosition;
            // }
            break;
        }
    }
}
void MEngineEditor::RenderToolbarPanel()
{
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_None);
    ImGui::Columns(2, "##toolbar", false);
    ImGui::SetColumnWidth(0, 500);
    // 显示工具栏按钮
    // ImGui::BeginGroup();
    // {
    //     if (ImGui::Button("Play"))
    //     {
    //         mIsPlay = !mIsPlay;
    //     }
    //     ImGui::SameLine();
    //     if (ImGui::Button("Pause"))
    //     {
    //         mIsPause = !mIsPause;
    //     }
    //     ImGui::SameLine();
    //     if (ImGui::Button("Stop"))
    //     {
    //         mIsStop = !mIsStop;
    //     }
    //     ImGui::EndGroup();
    // }
    // ImGui::SameLine();
    // 显示窗口标题 FPS 等信息
    ImGui::BeginGroup();
    {
        auto sceneWindow = ImGui::FindWindowByName("Viewport");
        ImGui::Text("SceneView Size: %1.f x %1.f", sceneWindow->ContentSize.x, sceneWindow->ContentSize.y);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %1.f", ImGui::GetIO().Framerate);
        if (ImGui::RadioButton("Translate", mGuizmoOperation == ImGuizmo::TRANSLATE) || ImGui::IsKeyDown(ImGuiKey_W))
            mGuizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mGuizmoOperation == ImGuizmo::ROTATE) || ImGui::IsKeyDown(ImGuiKey_E))
            mGuizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mGuizmoOperation == ImGuizmo::SCALE) || ImGui::IsKeyDown(ImGuiKey_R))
            mGuizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Local", mGuizmoMode == ImGuizmo::LOCAL))
            mGuizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mGuizmoMode == ImGuizmo::WORLD))
            mGuizmoMode = ImGuizmo::WORLD;
        ImGui::EndGroup();
    }
    ImGui::NextColumn();
    ImGui::BeginGroup();
    {
        if (ImGui::BeginCombo("Resolution", mCurrentResolution.ToString().c_str()))
        {
            for (Resolution &resolution : mResolutions)
            {
                if (ImGui::Selectable(resolution.ToString().data(), mCurrentResolution == resolution))
                {
                    mCurrentResolution = resolution;
                    mRenderSystem->CreateFrameBuffer(resolution.width, resolution.height);
                    auto &cameraComponent = mRegistry->get<CameraComponent>(mEditorCamera);
                    if (cameraComponent.isMainCamera)
                    {
                        cameraComponent.aspectRatio = static_cast<float>(resolution.width) / resolution.height;
                        cameraComponent.dirty = true;
                        break;
                    }
                }
            }
            ImGui::EndCombo();
        }
        ImGui::EndGroup();
    }
    ImGui::Columns(1);
    ImGui::End();
}
void MEngineEditor::RenderViewportPanel()
{
    ImGui::Begin("Viewport");
    auto renderSystem = injector.create<std::shared_ptr<RenderSystem>>();
    auto colorAttachment = renderSystem->ColorAttachment;
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    uint32_t width = static_cast<uint32_t>(windowSize.x);
    uint32_t height = static_cast<uint32_t>(windowSize.y);
    // 获取Camera
    // 获取相机组件
    auto &cameraComponent = mRegistry->get<CameraComponent>(mEditorCamera);
    float targetAspectRatio = cameraComponent.aspectRatio; // 或者固定值，如 16.0f / 9.0f
    float currentAspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // 计算按比例缩放后的图像显示大小
    ImVec2 displaySize = windowSize;
    if (currentAspectRatio > targetAspectRatio)
    {
        // 窗口太宽，限制宽度
        displaySize.x = windowSize.y * targetAspectRatio;
    }
    else
    {
        // 窗口太高，限制高度
        displaySize.y = windowSize.x / targetAspectRatio;
    }

    // 居中显示图像
    ImGui::SetCursorPos(ImVec2((windowSize.x - displaySize.x) * 0.5f, (windowSize.y - displaySize.y) * 0.5f));
    ImVec2 imagePos = ImGui::GetCursorScreenPos(); // 获取图像的实际屏幕坐标
    ImGui::Image((ImTextureID)(intptr_t)colorAttachment, displaySize, ImVec2(0, 1), ImVec2(1, 0));

    // 设置 ImGuizmo 绘制区域
    ImGuizmo::SetRect(imagePos.x, imagePos.y, displaySize.x, displaySize.y);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    if (mSelectedEntity != entt::null)
    {
        if (mRegistry->any_of<TransformComponent>(mSelectedEntity))
        {
            auto &transform = mRegistry->get<TransformComponent>(mSelectedEntity);
            auto modelMatrix = transform.modelMatrix;
            auto viewMatrix = cameraComponent.viewMatrix;
            ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(cameraComponent.projectionMatrix),
                                 mGuizmoOperation, mGuizmoMode, glm::value_ptr(modelMatrix));
            if (ImGuizmo::IsUsing())
            {
                // 分解矩阵
                glm::vec3 translation, scale, skew;
                glm::quat rotation;
                glm::vec4 perspective;
                if (glm::decompose(modelMatrix, scale, rotation, translation, skew, perspective))
                {
                    transform.localPosition = translation;
                    transform.localRotation = rotation;
                    transform.localScale = scale;

                    transform.dirty = true;
                }
            }
        }
    }
    ImGui::End();
}
void MEngineEditor::DeleteAsset(entt::entity entity)
{
    if (mRegistry->valid(entity))
    {
        auto &transform = mRegistry->get<TransformComponent>(entity);
        // 删除子节点
        auto children = transform.children;
        for (auto child : children)
        {
            if (mRegistry->valid(child))
            {
                DeleteAsset(child);
            }
        }
        if (transform.parent != entt::null)
        {
            auto &parentTransform = mRegistry->get<TransformComponent>(transform.parent);
            parentTransform.children.erase(
                std::remove(parentTransform.children.begin(), parentTransform.children.end(), entity),
                parentTransform.children.end());
        }
        // 删除当前节点
        mRegistry->destroy(entity);
    }
}
void MEngineEditor::RenderHierarchyPanel()
{
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_None);
    if (ImGui::BeginPopupContextWindow("HierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Create Empty"))
        {
            auto entity = mRegistry->create();
            auto &transform = mRegistry->emplace<TransformComponent>(entity);
        }
        if (ImGui::BeginMenu("Create"))
        {
            // if (ImGui::MenuItem("Cube"))
            // {
            //     auto entity = mRegistry->create();
            //     auto &transformComponent = mRegistry->emplace<TransformComponent>(entity);
            //     auto &meshComponent = mRegistry->emplace<MeshComponent>(entity);
            //     auto &materialComponent = mRegistry->emplace<MaterialComponent>(entity);
            //     materialComponent.type = MaterialType::PBR;
            //     materialComponent.materialID = mResourceManager->GetBasicPipeline(PipelineType::ForwardPBR)->ID;
            //     auto cube = mResourceManager->GetBasicGeometry(PrimitiveType::Cube);
            //     meshComponent.modelID = cube->ID;
            //     transformComponent.name = "Cube";
            // }
            // if (ImGui::MenuItem("Sphere"))
            // {
            //     auto entity = mRegistry->create();
            //     auto &transformComponent = mRegistry->emplace<TransformComponent>(entity);
            //     auto &meshComponent = mRegistry->emplace<MeshComponent>(entity);
            //     auto &materialComponent = mRegistry->emplace<MaterialComponent>(entity);
            //     auto sphere = mResourceManager->GetBasicGeometry(PrimitiveType::Sphere);
            //     meshComponent.modelID = sphere->ID;
            //     transformComponent.name = "Sphere";
            // }
            // if (ImGui::MenuItem("Cylinder"))
            // {
            //     auto entity = mRegistry->create();
            //     auto &transformComponent = mRegistry->emplace<TransformComponent>(entity);
            //     auto &meshComponent = mRegistry->emplace<MeshComponent>(entity);
            //     auto &materialComponent = mRegistry->emplace<MaterialComponent>(entity);
            //     auto cylinder = mResourceManager->GetBasicGeometry(PrimitiveType::Cylinder);
            //     meshComponent.modelID = cylinder->ID;
            //     transformComponent.name = "Cylinder";
            // }
            // if (ImGui::MenuItem("Quad"))
            // {
            //     auto entity = mRegistry->create();
            //     auto &transformComponent = mRegistry->emplace<TransformComponent>(entity);
            //     auto &meshComponent = mRegistry->emplace<MeshComponent>(entity);
            //     auto &materialComponent = mRegistry->emplace<MaterialComponent>(entity);
            //     auto quad = mResourceManager->GetBasicGeometry(PrimitiveType::Quad);
            //     meshComponent.modelID = quad->ID;
            //     transformComponent.name = "Quad";
            // }
            ImGui::EndMenu();
        }
        if (mSelectedEntity != entt::null && mRegistry->valid(mSelectedEntity))
        {
            if (ImGui::MenuItem("Delete"))
            {
                DeleteAsset(mSelectedEntity);
                mSelectedEntity = entt::null;
            }
        }
        ImGui::EndPopup();
    }
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
    {
        mSelectedEntity = entt::null;
    }
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
    for (auto entity : rootEntities)
    {
        RenderHierarchyItem(entity);
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
            entt::entity draggedAsset = *(const entt::entity *)payload->Data;

            auto &draggedTransform = mRegistry->get<TransformComponent>(draggedAsset);

            // 如果当前有父节点，则移除父节点关系
            if (draggedTransform.parent != entt::null)
            {
                auto &parentTransform = mRegistry->get<TransformComponent>(draggedTransform.parent);
                auto it = std::find(parentTransform.children.begin(), parentTransform.children.end(), draggedAsset);
                if (it != parentTransform.children.end())
                {
                    parentTransform.children.erase(it);
                }
                draggedTransform.parent = entt::null;
            }
        }
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_ITEM"))
        {
            IM_ASSERT(payload->DataSize == sizeof(mSelectedAsset));
            auto asset = *(const std::shared_ptr<Asset> *)payload->Data;
            // switch (asset->Type)
            // {
            // case AssetType::None:
            // case AssetType::Folder:
            // case AssetType::File:
            // case AssetType::Material:
            // case AssetType::Mesh:
            // case AssetType::PhongMaterial:
            // case AssetType::Texture2D:
            // case AssetType::TextureCube:
            // case AssetType::Model: {
            //     GetAssetFromModel(asset->ID, mRegistry);
            //     break;
            // }
            // case AssetType::Animation:
            // case AssetType::Shader:
            // case AssetType::Audio:
            // case AssetType::Pipeline:
            //     break;
            // }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopStyleColor(3);
    ImGui::End();
}
void MEngineEditor::RenderHierarchyItem(entt::entity entity)
{
    auto &transform = mRegistry->get<TransformComponent>(entity);
    std::string name = transform.name;
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    if (transform.children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }
    bool isSelected = (mSelectedEntity == entity);
    // 如果选中，设置背景色
    if (isSelected)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));        // 选中时的背景色
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.4f, 0.9f, 1.0f)); // 悬停时的背景色
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));  // 激活时的背景色
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)entity, flags, "%s", name.data());
    if (isSelected)
    {
        ImGui::PopStyleColor(3);
    }
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        mSelectedEntity = entity;
        mSelectedAsset = nullptr;
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        mSelectedEntity = entity;
        mSelectedAsset = nullptr;
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
            entt::entity draggedAsset = *(const entt::entity *)payload->Data;

            // 重新设置父子关系
            auto &draggedTransform = mRegistry->get<TransformComponent>(draggedAsset);
            auto &targetTransform = mRegistry->get<TransformComponent>(entity);
            if (draggedTransform.parent != entity)
            {
                // 移除拖拽实体的父节点
                if (draggedTransform.parent != entt::null)
                {
                    auto &parentTransform = mRegistry->get<TransformComponent>(draggedTransform.parent);
                    auto it = std::find(parentTransform.children.begin(), parentTransform.children.end(), draggedAsset);
                    if (it != parentTransform.children.end())
                    {
                        parentTransform.children.erase(it);
                    }
                }
                // 设置新的父节点
                draggedTransform.parent = entity;
                targetTransform.children.push_back(draggedAsset);
            }
        }
        ImGui::EndDragDropTarget();
    }

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
void MEngineEditor::RenderInspectorPanel()
{
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None);
    if (mSelectedEntity != entt::null)
    {
        auto entityLabel = std::format("Asset {}", static_cast<uint32_t>(mSelectedEntity));
        ImGui::Text("%s", entityLabel.c_str());
        if (mRegistry->any_of<AssetsComponent>(mSelectedEntity))
        {
            auto &assetComponent = mRegistry->get<AssetsComponent>(mSelectedEntity);
            InspectorUI(assetComponent);
        }
        if (mRegistry->any_of<TextureComponent>(mSelectedEntity))
        {
            auto &textureComponent = mRegistry->get<TextureComponent>(mSelectedEntity);
            InspectorUI(textureComponent);
        }
        if (mRegistry->any_of<TransformComponent>(mSelectedEntity))
        {
            auto &transformComponent = mRegistry->get<TransformComponent>(mSelectedEntity);
            InspectorUI(transformComponent);
        }
        if (mRegistry->any_of<CameraComponent>(mSelectedEntity))
        {
            auto &cameraComponent = mRegistry->get<CameraComponent>(mSelectedEntity);
            InspectorUI(cameraComponent);
        }
        if (mRegistry->any_of<MeshComponent>(mSelectedEntity))
        {
            auto &meshComponent = mRegistry->get<MeshComponent>(mSelectedEntity);
            InspectorUI(meshComponent);
        }
        // if (mRegistry->any_of<MaterialComponent>(mSelectedEntity))
        // {
        //     auto &materialComponent = mRegistry->get<MaterialComponent>(mSelectedEntity);
        //     InspectorUI(materialComponent);
        // }
        if (mRegistry->any_of<LightComponent>(mSelectedEntity))
        {
            auto &lightComponent = mRegistry->get<LightComponent>(mSelectedEntity);
            InspectorUI(lightComponent);
        }
    }
    if (mSelectedAsset != nullptr)
    {
        // auto assetLabel = std::format("Asset {}", mSelectedAsset->ID.ToString());
        // ImGui::Text("%s", assetLabel.c_str());
        // if (mSelectedAsset->Type == AssetType::Folder)
        // {
        //     auto entity = std::dynamic_pointer_cast<Folder>(mSelectedAsset);
        //     InspectorUI<Folder>(*entity);
        // }
    }
    // 右键菜单
    if (ImGui::BeginPopupContextWindow("AddComponentContextMenu", ImGuiPopupFlags_MouseButtonRight))
    {
        if (mSelectedEntity != entt::null)
        {
            if (ImGui::BeginMenu("Add Component"))
            {
                if (!mRegistry->any_of<TransformComponent>(mSelectedEntity))
                {
                    if (ImGui::MenuItem("Transform"))
                    {
                        auto &transformComponent = mRegistry->emplace<TransformComponent>(mSelectedEntity);
                    }
                }
                if (!mRegistry->any_of<CameraComponent>(mSelectedEntity))
                {
                    if (ImGui::MenuItem("Camera"))
                    {
                        auto &cameraComponent = mRegistry->emplace<CameraComponent>(mSelectedEntity);
                        cameraComponent.isMainCamera = false;
                    }
                }
                if (!mRegistry->any_of<MeshComponent>(mSelectedEntity))
                {
                    if (ImGui::MenuItem("Mesh"))
                    {
                        auto &meshComponent = mRegistry->emplace<MeshComponent>(mSelectedEntity);
                    }
                }
                // if (!mRegistry->any_of<MaterialComponent>(mSelectedEntity))
                // {
                //     if (ImGui::MenuItem("Material"))
                //     {
                //         auto &materialComponent = mRegistry->emplace<MaterialComponent>(mSelectedEntity);
                //     }
                // }
                if (!mRegistry->any_of<LightComponent>(mSelectedEntity))
                {
                    if (ImGui::MenuItem("Light"))
                    {
                        auto &lightComponent = mRegistry->emplace<LightComponent>(mSelectedEntity);
                    }
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::End();
}

void MEngineEditor::RenderAssetPanel()
{
    ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_None);
    if (ImGui::Button("<-"))
    {
        if (mCurrentPath != mProjectPath)
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
    auto currentPathAssets = AssetDatabase::GetAllAssets();
    for (auto asset : currentPathAssets)
    {
        ImTextureID textureID = 0;
        // if (mAssetIcons.find(asset->Type) != mAssetIcons.end())
        // {
        //     textureID = mAssetIcons[asset->Type];
        // }
        if (ImGui::Selectable("##btn", mSelectedAsset == asset,
                              ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick,
                              ImVec2(mAssetIconSize + 10, mAssetIconSize + 20)))
        {
            mSelectedAsset = asset;
            mSelectedEntity = entt::null;
            if (ImGui::IsMouseDoubleClicked(0))
            {
                if (typeid(*asset) == typeid(Folder))
                {
                    mCurrentPath = asset->importer->assetPath;
                }
            }
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        {
            mHoveredAsset = asset;
            mHoveredEntity = entt::null;
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                mSelectedAsset = asset;
                mHoveredEntity = entt::null;
            }
            if (ImGui::IsMouseDragging(0))
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    // 设置拖拽数据
                    ImGui::SetDragDropPayload("ASSET_ITEM", &asset, sizeof(asset));
                    // 显示拖拽预览（图标）
                    ImGui::Image(textureID, ImVec2(0, 1), ImVec2(1, 0));
                    // 可选：添加文字说明
                    ImGui::Text("Drag %s", asset->ID.ToString().c_str());
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
        ImGui::Text("%s", asset->importer->assetPath.stem().string().c_str());
        ImGui::NextColumn(); // 移动到下一列
    }
    ImGui::Columns(1); // 恢复单列模式

    if (ImGui::BeginPopupContextWindow("AssetContextMenu", ImGuiPopupFlags_MouseButtonRight))
    {

        if (ImGui::BeginMenu("Create Material"))
        {
            // if (ImGui::MenuItem("PBR"))
            // {
            //     auto pbrMaterial = mResourceManager->CreateAsset<PBRMaterial>();
            //     auto path = mResourceManager->GenerateUniquePath<PBRMaterial>(mProjectPath, "New Material");
            //     mResourceManager->SaveAsset<PBRMaterial>(pbrMaterial->ID, path);
            // }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    ImGui::End();
}
void MEngineEditor::LoadUIResources()
{
    // auto foldTexture = mResourceManager->CreateAsset<Texture2D>();
    // foldTexture->ImagePath = mUIResourcesPath / "folder.png";
    // auto fileTexture = mResourceManager->CreateAsset<Texture2D>();
    // fileTexture->ImagePath = mUIResourcesPath / "file.png";
    // mResourceManager->UpdateAsset(foldTexture);
    // mResourceManager->UpdateAsset(fileTexture);
    // mAssetIcons[AssetType::Folder] = foldTexture->mTextureID;
    // mAssetIcons[AssetType::File] = fileTexture->mTextureID;
    // LogInfo("Loaded UI resources");
}
void MEngineEditor::GetAssetFromModel(const UUID &modelID, std::shared_ptr<entt::registry> registry)
{
    // auto model = mResourceManager->GetAsset<Model>(modelID);
    // auto &meshes = model->Meshes;
    // auto RootNode = model->RootNode;
    // std::function<entt::entity(std::shared_ptr<Node> node)> traverseNode = [&](std::shared_ptr<Node> node) {
    //     auto currentNode = registry->create();
    //     auto &currentNodeTransformComponent = registry->emplace<TransformComponent>(currentNode);
    //     currentNodeTransformComponent.name = node->Name;
    //     auto transform = node->Transform;
    //     TransformSystem::SetModelMatrix(currentNodeTransformComponent, transform);
    //     // TODO: 添加根据modelmatrix计算localposition,localrotation,localscale
    //     for (auto index : node->Meshes)
    //     {
    //         auto mesh = meshes[index];
    //         auto entity = registry->create();
    //         auto &meshComponent = registry->emplace<MeshComponent>(entity);
    //         auto &materialComponent = registry->emplace<MaterialComponent>(entity);
    //         auto &transformComponent = registry->emplace<TransformComponent>(entity);
    //         transformComponent.parent = currentNode;
    //         meshComponent.modelID = model->ID;
    //         meshComponent.meshIndex = index;
    //         currentNodeTransformComponent.children.push_back(entity);
    //     }
    //     for (auto child : node->Children)
    //     {
    //         auto childAsset = traverseNode(child);
    //         auto &childTransformComponent = registry->get<TransformComponent>(childAsset);
    //         childTransformComponent.parent = currentNode;
    //         currentNodeTransformComponent.children.push_back(childAsset);
    //         auto childTransform = child->Transform;
    //         TransformSystem::SetModelMatrix(childTransformComponent, childTransform,
    //                                         currentNodeTransformComponent.modelMatrix);
    //     }
    //     return currentNode;
    // };
    // traverseNode(RootNode);
}
} // namespace MEngine
