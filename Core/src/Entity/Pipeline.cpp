#include "Entity/Pipeline.hpp"
#include "Logger.hpp"

namespace MEngine
{
Pipeline::Pipeline()
{
    Name = "Pipeline";
    Type = EntityType::Pipeline;
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    programID = glCreateProgram();
}
Pipeline::~Pipeline()
{
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    glDeleteShader(geometryShaderID);
}
GLuint Pipeline::LoadShader(GLuint shaderID, const std::filesystem::path &shaderPath)
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
void Pipeline::Update()
{
    if (std::filesystem::exists(VertexShaderPath))
    {
        vertexShaderID = LoadShader(vertexShaderID, VertexShaderPath);
        glAttachShader(programID, vertexShaderID);
    }
    else
    {
        LogWarn("Vertex shader file does not exist, SKIPPED!!!");
    }
    if (std::filesystem::exists(FragmentShaderPath))
    {
        fragmentShaderID = LoadShader(fragmentShaderID, FragmentShaderPath);
        glAttachShader(programID, fragmentShaderID);
    }
    else
    {
        LogWarn("Fragment shader file does not exist, SKIPPED!!!");
    }
    if (std::filesystem::exists(GeometryShaderPath))
    {
        geometryShaderID = LoadShader(geometryShaderID, GeometryShaderPath);
        glAttachShader(programID, geometryShaderID);
    }
    else
    {
        LogWarn("Geometry shader file does not exist, SKIPPED!!!");
    }
    glLinkProgram(programID);
}
} // namespace MEngine