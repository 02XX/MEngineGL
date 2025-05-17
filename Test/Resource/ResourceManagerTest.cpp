#include "ResourceManager.hpp"
#include "Entity/Pipeline.hpp"
#include "gtest/gtest.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glad/glad.h>
#include <gtest/gtest.h>

using namespace MEngine;
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

TEST_F(ResourceManagerTest, CreateShaderAsset)
{
    auto id = mResourceManager.CreateAsset<Pipeline>(mTestPath);
    auto shader = mResourceManager.GetAsset<Pipeline>(id);
    EXPECT_NE(id, UUID());
    EXPECT_NE(shader, nullptr);
    auto sourcePath = shader->GetPath();
    EXPECT_TRUE(std::filesystem::exists(sourcePath));
}
TEST_F(ResourceManagerTest, LoadShaderAsset)
{
    auto id = mResourceManager.CreateAsset<Pipeline>(mTestPath);
    auto shader = mResourceManager.GetAsset<Pipeline>(id);
    EXPECT_NE(id, UUID());
    EXPECT_NE(shader, nullptr);
    auto sourcePath = shader->GetPath();
    EXPECT_TRUE(std::filesystem::exists(sourcePath));
    id = mResourceManager.LoadAsset(sourcePath);
    EXPECT_NE(id, UUID());
    auto loadedShader = mResourceManager.GetAsset<Pipeline>(id);
    EXPECT_NE(loadedShader, nullptr);
    EXPECT_EQ(shader->GetID(), loadedShader->GetID());
}
TEST_F(ResourceManagerTest, CreatePBRMaterialAsset)
{
    auto id = mResourceManager.CreateAsset<PBRMaterial>(mTestPath);
    auto material = mResourceManager.GetAsset<PBRMaterial>(id);
    EXPECT_NE(id, UUID());
    EXPECT_NE(material, nullptr);
    auto sourcePath = material->GetPath();
    EXPECT_TRUE(std::filesystem::exists(sourcePath));
}
TEST_F(ResourceManagerTest, LoadPBRMaterialAsset)
{
    auto id = mResourceManager.CreateAsset<PBRMaterial>(mTestPath);
    auto material = mResourceManager.GetAsset<PBRMaterial>(id);
    EXPECT_NE(id, UUID());
    EXPECT_NE(material, nullptr);
    auto sourcePath = material->GetPath();
    EXPECT_TRUE(std::filesystem::exists(sourcePath));
    id = mResourceManager.LoadAsset(sourcePath);
    EXPECT_NE(id, UUID());
    auto loadedMaterial = mResourceManager.GetAsset<PBRMaterial>(id);
    EXPECT_NE(loadedMaterial, nullptr);
    EXPECT_EQ(material->GetID(), loadedMaterial->GetID());
}
TEST_F(ResourceManagerTest, CreateTextureAsset)
{
    GTEST_LOG_(INFO) << "0\n";
    auto id = mResourceManager.CreateAsset<Texture2D>(mTestPath);
    GTEST_LOG_(INFO) << "1\n";
    auto texture = mResourceManager.GetAsset<Texture2D>(id);
    GTEST_LOG_(INFO) << "2\n";
    EXPECT_NE(id, UUID());
    EXPECT_NE(texture, nullptr);
    GTEST_LOG_(INFO) << "3\n";
    auto sourcePath = texture->GetPath();
    GTEST_LOG_(INFO) << "4\n";
    EXPECT_TRUE(std::filesystem::exists(sourcePath));
    GTEST_LOG_(INFO) << "5\n";
}
TEST_F(ResourceManagerTest, UpdateTextureAsset)
{
    auto id = mResourceManager.CreateAsset<Texture2D>(mTestPath);
    auto texture = mResourceManager.GetAsset<Texture2D>(id);
    EXPECT_NE(id, UUID());
    EXPECT_NE(texture, nullptr);
    texture->SetImagePath(mTestPath / "file.png");
    mResourceManager.UpdateAsset(id, texture);
    EXPECT_NE(texture->GetTextureID(), 0);
}
TEST_F(ResourceManagerTest, LoadTextureAsset)
{
    std::filesystem::path imagePath = mTestPath / "file.tex2d";
    if (!std::filesystem::exists(imagePath))
    {
        GTEST_LOG_(WARNING) << "Image file does not exist: " << imagePath.string()
                            << ", skip ResourceManagerTest.LoadTextureAsset" << "\n";
        return;
    }
    auto id = mResourceManager.LoadAsset(imagePath);
    auto texture = mResourceManager.GetAsset<Texture2D>(id);
    EXPECT_NE(id, UUID());
    EXPECT_NE(texture, nullptr);
    EXPECT_NE(texture->GetTextureID(), 0);
    EXPECT_TRUE(std::filesystem::exists(texture->GetPath()));
    EXPECT_NE(texture->GetWidth(), 0);
    EXPECT_NE(texture->GetHeight(), 0);
    EXPECT_NE(texture->GetTextureID(), 0);
    EXPECT_NE(texture->GetSamplerID(), 0);
}