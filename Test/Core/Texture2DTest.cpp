#include "Entity/Texture2D.hpp"
#include "gtest/gtest.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glad/glad.h>
#include <gtest/gtest.h>

using namespace MEngine;
class Texture2DTest : public ::testing::Test
{
  protected:
    std::filesystem::path mTestPath = std::filesystem::current_path() / "Test";
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
TEST_F(Texture2DTest, CreateTexture2D)
{
    auto texture = std::make_shared<Texture2D>();
    texture->SetImagePath(std::filesystem::current_path() / "file.png");
    EXPECT_NE(texture->GetTextureID(), 0);
    GTEST_LOG_(INFO) << "Texture2D ID: " << texture->GetTextureID();
}