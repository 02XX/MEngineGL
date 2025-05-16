#include "Manager/ShaderManager.hpp"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glad/glad.h>
#include <gtest/gtest.h>

using namespace MEngine;
class ShaderManagerTest : public ::testing::Test
{
  protected:
    MEngine::ShaderManager mShaderManager;

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

TEST_F(ShaderManagerTest, Create)
{
    auto shader = mShaderManager.Create();
    EXPECT_NE(shader, nullptr);
    EXPECT_NE(shader->GetID().ToString().find("Shader_"), 0);
}
TEST_F(ShaderManagerTest, Update)
{
    auto shader = mShaderManager.Create();
    EXPECT_NE(shader, nullptr);
    std::filesystem::path vertexShaderPath = std::filesystem::current_path() / "Shader" / "forward.vert";
    std::filesystem::path fragmentShaderPath = std::filesystem::current_path() / "Shader" / "forward.frag";
    shader->SetVertexShader(vertexShaderPath);
    shader->SetFragmentShader(fragmentShaderPath);
    mShaderManager.Update(shader->GetID(), *shader);
    EXPECT_NE(shader->GetVertexShader(), 0);
    EXPECT_NE(shader->GetFragmentShader(), 0);
    EXPECT_EQ(shader->GetGeometryShader(), 0);
    EXPECT_EQ(shader->GetTessControlShader(), 0);
    EXPECT_EQ(shader->GetTessEvalShader(), 0);
    EXPECT_EQ(shader->GetComputeShader(), 0);
}
TEST_F(ShaderManagerTest, Save)
{
    auto shader = mShaderManager.Create();
    EXPECT_NE(shader, nullptr);
    std::filesystem::path vertexShaderPath = std::filesystem::current_path() / "Shader" / "forward.vert";
    std::filesystem::path fragmentShaderPath = std::filesystem::current_path() / "Shader" / "forward.frag";
    shader->SetVertexShader(vertexShaderPath);
    shader->SetFragmentShader(fragmentShaderPath);
    mShaderManager.Update(shader->GetID(), *shader);
    mShaderManager.Save(std::filesystem::current_path() / "shaderTest.shader", *shader);
}
TEST_F(ShaderManagerTest, Load)
{
    const std::filesystem::path shaderPath = std::filesystem::current_path() / "shaderTest.shader";
    auto shaderID = mShaderManager.Load(shaderPath);
    auto shader = mShaderManager.GetById(shaderID);
    EXPECT_NE(shader->GetVertexShader(), 0);
    EXPECT_NE(shader->GetFragmentShader(), 0);
    EXPECT_EQ(shader->GetGeometryShader(), 0);
    EXPECT_EQ(shader->GetTessControlShader(), 0);
    EXPECT_EQ(shader->GetTessEvalShader(), 0);
    EXPECT_EQ(shader->GetComputeShader(), 0);
}