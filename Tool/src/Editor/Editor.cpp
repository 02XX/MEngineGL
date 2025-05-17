#include "Editor/Editor.hpp"
#include "IConfigure.hpp"
#include "Logger.hpp"
#include <GLFW/glfw3.h>
#include <boost/di.hpp>
#include <memory>
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
    LogInfo("Editor initialized");
}
Editor::~Editor()
{
    LogInfo("Editor destroyed");
}
void Editor::Init()
{
    InitWindow();
    InitOpenGL();
    InitSystems();
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
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        if (mIsRunning)
        {
            for (auto &system : mSystems)
            {
                system->Update(deltaTime);
            }
        }
        glfwSwapBuffers(mWindow);
    }
}
void Editor::Shutdown()
{
    for (auto &system : mSystems)
    {
        system->Shutdown();
    }
}

} // namespace MEngine
