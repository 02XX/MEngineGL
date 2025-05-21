#include "Editor/Editor.hpp"
#include "BasicGeometryFactory.hpp"
#include "Component/TransformComponent.hpp"
#include "Entity/Mesh.hpp"
#include "Logger.hpp"
#include "UUID.hpp"
#include <boost/di.hpp>
#include <entt/entity/entity.hpp>
#include <imgui.h>
namespace MEngine
{
namespace DI = boost::di;
auto injector = DI::make_injector(DI::bind<IConfigure>().to<Configure>().in(DI::unique),
                                  DI::bind<ResourceManager>().to<ResourceManager>().in(DI::singleton),
                                  DI::bind<RenderSystem>().to<RenderSystem>().in(DI::singleton),
                                  DI::bind<TransformSystem>().to<TransformSystem>().in(DI::singleton),
                                  DI::bind<CameraSystem>().to<CameraSystem>().in(DI::singleton),
                                  DI::bind<entt::registry>().to<entt::registry>().in(DI::singleton),
                                  DI::bind<BasicGeometryFactory>().to<BasicGeometryFactory>().in(DI::singleton));

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
    mRegistry = injector.create<std::shared_ptr<entt::registry>>();
    mResourceManager = injector.create<std::shared_ptr<ResourceManager>>();
    mBasicGeometryFactory = injector.create<std::shared_ptr<BasicGeometryFactory>>();
    mRenderSystem = injector.create<std::shared_ptr<RenderSystem>>();
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
    LoadAssets(mProjectPath);
    mResourceManager->CreateDefault();
    // camera
    auto camera = mRegistry->create();
    auto &cameraComponent = mRegistry->emplace<CameraComponent>(camera);
    auto &transformComponent = mRegistry->emplace<TransformComponent>(camera);
    cameraComponent.isEditorCamera = true;
    cameraComponent.aspectRatio = 16.0f / 9.0f;
    transformComponent.localPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    transformComponent.name = "EditorCamera";

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

    ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
    ImGuizmo::Enable(true);
}
void Editor::InitSystems()
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
void Editor::Update(float deltaTime)
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
void Editor::EditorCamera()
{
    // 获取相机组件
    auto view = mRegistry->view<CameraComponent>();
    for (auto entity : view)
    {
        auto &cameraComponent = view.get<CameraComponent>(entity);
        if (cameraComponent.isEditorCamera)
        {
            mEditorCamera = entity;
            // if (mSelectedEntity != entt::null)
            // {
            //     auto &transformComponent = mRegistry->get<TransformComponent>(mSelectedEntity);
            //     cameraComponent.target = transformComponent.worldPosition;
            // }
            break;
        }
    }
}
void Editor::RenderToolbarPanel()
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
void Editor::RenderViewportPanel()
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
void Editor::RenderHierarchyPanel()
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
            if (ImGui::MenuItem("Cube"))
            {
                auto entity = mRegistry->create();
                auto &transformComponent = mRegistry->emplace<TransformComponent>(entity);
                auto &meshComponent = mRegistry->emplace<MeshComponent>(entity);
                auto &materialComponent = mRegistry->emplace<MaterialComponent>(entity);
                auto cube = mResourceManager->GetBasicGeometry(PrimitiveType::Cube);
                meshComponent.meshID = cube->ID;
                transformComponent.name = "Cube";
            }
            if (ImGui::MenuItem("Sphere"))
            {
                auto entity = mRegistry->create();
                auto &transformComponent = mRegistry->emplace<TransformComponent>(entity);
                auto &meshComponent = mRegistry->emplace<MeshComponent>(entity);
                auto &materialComponent = mRegistry->emplace<MaterialComponent>(entity);
                auto sphere = mResourceManager->GetBasicGeometry(PrimitiveType::Sphere);
                meshComponent.meshID = sphere->ID;
                transformComponent.name = "Sphere";
            }
            if (ImGui::MenuItem("Cylinder"))
            {
                auto entity = mRegistry->create();
                auto &transformComponent = mRegistry->emplace<TransformComponent>(entity);
                auto &meshComponent = mRegistry->emplace<MeshComponent>(entity);
                auto &materialComponent = mRegistry->emplace<MaterialComponent>(entity);
                auto cylinder = mResourceManager->GetBasicGeometry(PrimitiveType::Cylinder);
                meshComponent.meshID = cylinder->ID;
                transformComponent.name = "Cylinder";
            }
            if (ImGui::MenuItem("Quad"))
            {
                auto entity = mRegistry->create();
                auto &transformComponent = mRegistry->emplace<TransformComponent>(entity);
                auto &meshComponent = mRegistry->emplace<MeshComponent>(entity);
                auto &materialComponent = mRegistry->emplace<MaterialComponent>(entity);
                auto quad = mResourceManager->GetBasicGeometry(PrimitiveType::Quad);
                meshComponent.meshID = quad->ID;
                transformComponent.name = "Quad";
            }
            ImGui::EndMenu();
        }
        if (mSelectedEntity != entt::null && mRegistry->valid(mSelectedEntity))
        {
            if (ImGui::MenuItem("Delete"))
            {
                auto &transform = mRegistry->get<TransformComponent>(mSelectedEntity);
                // 删除子节点
                for (auto child : transform.children)
                {
                    if (mRegistry->valid(child))
                    {
                        mRegistry->destroy(child);
                    }
                }
                if (transform.parent != entt::null)
                {
                    auto &parentTransform = mRegistry->get<TransformComponent>(transform.parent);
                    parentTransform.children.erase(
                        std::remove(parentTransform.children.begin(), parentTransform.children.end(), mSelectedEntity),
                        parentTransform.children.end());
                }
                // 删除当前节点
                mRegistry->destroy(mSelectedEntity);
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
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
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
        if (mRegistry->any_of<CameraComponent>(mSelectedEntity))
        {
            auto &cameraComponent = mRegistry->get<CameraComponent>(mSelectedEntity);
            ComponentUI(cameraComponent);
        }
        if (mRegistry->any_of<MeshComponent>(mSelectedEntity))
        {
            auto &meshComponent = mRegistry->get<MeshComponent>(mSelectedEntity);
            ComponentUI(meshComponent);
        }
        if (mRegistry->any_of<MaterialComponent>(mSelectedEntity))
        {
            auto &materialComponent = mRegistry->get<MaterialComponent>(mSelectedEntity);
            ComponentUI(materialComponent);
        }
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
                if (!mRegistry->any_of<MaterialComponent>(mSelectedEntity))
                {
                    if (ImGui::MenuItem("Material"))
                    {
                        auto &materialComponent = mRegistry->emplace<MaterialComponent>(mSelectedEntity);
                    }
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::End();
}

void Editor::RenderAssetPanel()
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
    auto foldTexture = mResourceManager->CreateAsset<Texture2D>();
    foldTexture->ImagePath = mUIResourcesPath / "folder.png";
    auto fileTexture = mResourceManager->CreateAsset<Texture2D>();
    fileTexture->ImagePath = mUIResourcesPath / "file.png";
    mResourceManager->UpdateAsset(foldTexture);
    mResourceManager->UpdateAsset(fileTexture);
    mAssetIcons[AssetType::Folder] = foldTexture->mTextureID;
    mAssetIcons[AssetType::File] = fileTexture->mTextureID;
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
                    textureComponent.textureID = asset->ID;
                }
            }
            else
            {
                auto assetPath = entry.path();
                type = mResourceManager->GetRawTypeFromExtension(extension);
                assetPath.replace_extension(mResourceManager->GetAssetExtensionFromType(type));
                if (!std::filesystem::exists(assetPath))
                {
                    mResourceManager->LoadRawAsset(entry.path());
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
