#include "ResourceManager.hpp"
#include "Entity/Shader.hpp"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glad/glad.h>
#include <gtest/gtest.h>

using namespace MEngine;
class ResourceManagerTest : public ::testing::Test
{
  protected:
    MEngine::ResourceManager mResourceManager;

    void SetUp() override
    {
        if (!glfwInit())
        {
            throw std::runtime_error("GLFW init failed");
        }
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // 离屏窗口
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

TEST_F(ResourceManagerTest, CreateAsset)
{
    auto id = mResourceManager.CreateAsset<Shader>();
    auto shader = mResourceManager.GetAsset<Shader>(id);
    EXPECT_NE(id, UUID());
    EXPECT_NE(shader, nullptr);
    auto sourcePath = shader->GetPath();
    EXPECT_TRUE(std::filesystem::exists(sourcePath));
}
TEST_F(ResourceManagerTest, LoadAsset)
{
    auto id = mResourceManager.CreateAsset<Shader>();
    auto shader = mResourceManager.GetAsset<Shader>(id);
    EXPECT_NE(id, UUID());
    EXPECT_NE(shader, nullptr);
    auto sourcePath = shader->GetPath();
    EXPECT_TRUE(std::filesystem::exists(sourcePath));
    id = mResourceManager.LoadAsset(sourcePath);
    EXPECT_NE(id, UUID());
    auto loadedShader = mResourceManager.GetAsset<Shader>(id);
    EXPECT_NE(loadedShader, nullptr);
    EXPECT_EQ(shader->GetID(), loadedShader->GetID());
}