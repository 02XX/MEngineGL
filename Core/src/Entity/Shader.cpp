#include "Entity/Shader.hpp"
#include "Logger.hpp"

namespace MEngine
{
Shader::Shader()
{
    mName = "Shader";
    mVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    mFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    mGeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
}
Shader::~Shader()
{
    glDeleteShader(mVertexShaderID);
    glDeleteShader(mFragmentShaderID);
    glDeleteShader(mGeometryShaderID);
}
void Shader::SetVertexShader(const std::filesystem::path &vertexShaderPath)
{
    mVertexShaderPath = vertexShaderPath;
    if (std::filesystem::exists(mVertexShaderPath))
    {
        mVertexShaderID = LoadShader(mVertexShaderID, mVertexShaderPath);
    }
    else
    {
        LogWarn("Vertex shader file does not exist, SKIPPED!!!");
    }
}
void Shader::SetFragmentShader(const std::filesystem::path &fragmentShaderPath)
{
    mFragmentShaderPath = fragmentShaderPath;
    if (std::filesystem::exists(mFragmentShaderPath))
    {
        mFragmentShaderID = LoadShader(mFragmentShaderID, mFragmentShaderPath);
    }
    else
    {
        LogWarn("Fragment shader file does not exist, SKIPPED!!!");
    }
}
void Shader::SetGeometryShader(const std::filesystem::path &geometryShaderPath)
{
    mGeometryShaderPath = geometryShaderPath;
    if (std::filesystem::exists(mGeometryShaderPath))
    {
        mGeometryShaderID = LoadShader(mGeometryShaderID, mGeometryShaderPath);
    }
    else
    {
        LogWarn("Geometry shader file does not exist, SKIPPED!!!");
    }
}

GLuint Shader::LoadShader(GLuint shaderID, const std::filesystem::path &shaderPath)
{
    if (!std::filesystem::exists(shaderPath))
    {
        return 0;
    }
    std::ifstream file(shaderPath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open shader file: " + shaderPath.string());
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string shaderCodeStr = buffer.str();
    const char *shaderCode = shaderCodeStr.c_str();

    glShaderSource(shaderID, 1, &shaderCode, nullptr);
    glCompileShader(shaderID);
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint logLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> infoLog(logLength);
        glGetShaderInfoLog(shaderID, logLength, nullptr, infoLog.data());
        throw std::runtime_error("Failed to compile vertex shader: " + std::string(infoLog.data()));
    }
    return shaderID;
}
} // namespace MEngine