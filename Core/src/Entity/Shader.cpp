#include "Entity/Shader.hpp"

namespace MEngine
{
Shader::Shader()
{
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
    mVertexShaderID = LoadShader(mVertexShaderID, mVertexShaderPath);
}
void Shader::SetFragmentShader(const std::filesystem::path &fragmentShaderPath)
{
    mFragmentShaderPath = fragmentShaderPath;
    mFragmentShaderID = LoadShader(mFragmentShaderID, mFragmentShaderPath);
}
void Shader::SetGeometryShader(const std::filesystem::path &geometryShaderPath)
{
    mGeometryShaderPath = geometryShaderPath;
    mGeometryShaderID = LoadShader(mGeometryShaderID, mGeometryShaderPath);
}

GLuint Shader::LoadShader(GLuint shaderID, const std::filesystem::path &shaderPath)
{
    if (!std::filesystem::exists(shaderPath))
    {
        throw std::runtime_error("Shader file does not exist: " + shaderPath.string());
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