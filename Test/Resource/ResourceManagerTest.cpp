#include "ResourceManager.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Entity/Pipeline.hpp"
#include "Entity/Texture2D.hpp"
#include "gtest/gtest.h"
#include <GLFW/glfw3.h>
#include <entt/core/any.hpp>
#include <entt/meta/meta.hpp>
#include <filesystem>
#include <glad/glad.h>
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine;
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
class ResourceManagerTest : public ::testing::Test
{
  protected:
    MEngine::ResourceManager mResourceManager;
    std::filesystem::path mTestPath = std::filesystem::current_path() / "Test";
    void SetUp() override
    {
        if (!std::filesystem::exists(mTestPath))
        {
            std::filesystem::create_directories(mTestPath);
        }
        if (!glfwInit())
        {
            throw std::runtime_error("GLFW init failed");
        }
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);           // 离屏窗口
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // 启用调试上下文
        GLFWwindow *window = glfwCreateWindow(640, 480, "", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            throw std::runtime_error("Window creation failed");
        }
        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwTerminate();
            throw std::runtime_error("Failed to load OpenGL functions");
        }
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // 确保回调在错误发生的线程调用
        glDebugMessageCallback(DebugCallback, nullptr);
    }

    void TearDown() override
    {
        GLFWwindow *window = glfwGetCurrentContext();
        if (window)
        {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }
};

// TEST_F(ResourceManagerTest, CreateShaderAsset)
// {
//     auto shader = mResourceManager.CreateAsset<Pipeline>(mTestPath);
//     EXPECT_NE(shader->ID, UUID());
//     EXPECT_NE(shader, nullptr);
//     auto sourcePath = shader->SourcePath;
//     EXPECT_TRUE(std::filesystem::exists(sourcePath));
// }
// TEST_F(ResourceManagerTest, LoadShaderAsset)
// {
//     auto shader = mResourceManager.CreateAsset<Pipeline>(mTestPath);
//     EXPECT_NE(shader->ID, UUID());
//     EXPECT_NE(shader, nullptr);
//     auto sourcePath = shader->SourcePath;
//     EXPECT_TRUE(std::filesystem::exists(sourcePath));
//     auto loadedShader = mResourceManager.LoadAsset<Pipeline>(sourcePath);
//     EXPECT_NE(loadedShader->ID, UUID());
//     EXPECT_NE(loadedShader, nullptr);
//     EXPECT_EQ(shader->ID, loadedShader->ID);
// }
// TEST_F(ResourceManagerTest, CreatePBRMaterialAsset)
// {
//     auto material = mResourceManager.CreateAsset<PBRMaterial>(mTestPath);
//     EXPECT_NE(material->ID, UUID());
//     EXPECT_NE(material, nullptr);
//     auto sourcePath = material->SourcePath;
//     EXPECT_TRUE(std::filesystem::exists(sourcePath));
// }
// TEST_F(ResourceManagerTest, LoadPBRMaterialAsset)
// {
//     auto material = mResourceManager.CreateAsset<PBRMaterial>(mTestPath);
//     EXPECT_NE(material->ID, UUID());
//     EXPECT_NE(material, nullptr);
//     auto sourcePath = material->SourcePath;
//     EXPECT_TRUE(std::filesystem::exists(sourcePath));
//     auto loadedMaterial = mResourceManager.LoadAsset<PBRMaterial>(sourcePath);
//     EXPECT_NE(material->ID, UUID());
//     EXPECT_NE(loadedMaterial, nullptr);
//     EXPECT_EQ(material->ID, loadedMaterial->ID);
// }
// TEST_F(ResourceManagerTest, CreateTextureAsset)
// {
//     auto texture = mResourceManager.CreateAsset<Texture2D>(mTestPath);
//     EXPECT_NE(texture->ID, UUID());
//     EXPECT_NE(texture, nullptr);
//     auto sourcePath = texture->SourcePath;
//     EXPECT_TRUE(std::filesystem::exists(sourcePath));
// }
// TEST_F(ResourceManagerTest, UpdateTextureAsset)
// {
//     auto texture = mResourceManager.CreateAsset<Texture2D>(mTestPath);
//     EXPECT_NE(texture->ID, UUID());
//     EXPECT_NE(texture, nullptr);
//     texture->ImagePath = mTestPath / "file.png";
//     mResourceManager.UpdateAsset(texture->ID, texture);
//     EXPECT_NE(texture->mTextureID, 0);
// }
// TEST_F(ResourceManagerTest, LoadTextureAsset)
// {
//     std::filesystem::path imagePath = mTestPath / "file.tex2d";
//     if (!std::filesystem::exists(imagePath))
//     {
//         GTEST_LOG_(WARNING) << "Image file does not exist: " << imagePath.string()
//                             << ", skip ResourceManagerTest.LoadTextureAsset" << "\n";
//         return;
//     }
//     auto texture = mResourceManager.LoadAsset<Texture2D>(imagePath);
//     EXPECT_NE(texture->ID, UUID());
//     EXPECT_NE(texture, nullptr);
//     EXPECT_NE(texture->mTextureID, 0);
//     EXPECT_TRUE(std::filesystem::exists(texture->SourcePath));
//     EXPECT_NE(texture->Width, 0);
//     EXPECT_NE(texture->Height, 0);
//     EXPECT_NE(texture->mTextureID, 0);
//     EXPECT_NE(texture->mSamplerID, 0);
// }
// TEST_F(ResourceManagerTest, ModifyTextureAsset)
// {
//     auto texute = mResourceManager.CreateAsset<Texture2D>(mTestPath);
//     EXPECT_NE(texute->ID, UUID());
//     auto metaAny = entt::forward_as_meta(texute);
//     GTEST_LOG_(INFO) << "type: " << metaAny.type().info().name() << "\n";
// }