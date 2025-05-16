#include "Manager/ShaderManager.hpp"
#include "Logger.hpp"

namespace MEngine
{
std::shared_ptr<Shader> ShaderManager::Create()
{
    auto shader = std::make_shared<Shader>();
    UUID id = UUIDGenerator()();
    shader->mID = id;
    shader->mName = "Shader_" + id.ToString();
    mEntities[id] = shader;
    return shader;
}
void ShaderManager::Update(const UUID &id, const Shader &entity)
{
    auto it = mEntities.find(id);
    if (it != mEntities.end())
    {
        auto shader = it->second;
        shader->mVertexShaderID = LoadShader(shader->mVertexShaderPath);
        shader->mFragmentShaderID = LoadShader(shader->mFragmentShaderPath);
        shader->mGeometryShaderID = LoadShader(shader->mGeometryShaderPath);
        shader->mTessControlShaderID = LoadShader(shader->mTessControlShaderPath);
        shader->mTessEvalShaderID = LoadShader(shader->mTessEvalShaderPath);
        shader->mComputeShaderID = LoadShader(shader->mComputeShaderPath);
    }
    else
    {
        LogError("Failed to update shader: Shader not found");
    }
}
GLuint ShaderManager::LoadShader(const std::filesystem::path &shaderPath)
{
    if (!std::filesystem::exists(shaderPath))
    {
        LogWarn("Shader file does not exist: {}, skipped!", shaderPath.string());
        return 0;
    }
    std::ifstream file(shaderPath);
    if (!file.is_open())
    {
        LogError("Failed to open shader file: {}", shaderPath.string());
        return 0;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string shaderCodeStr = buffer.str();
    const char *shaderCode = shaderCodeStr.c_str();
    GLuint shaderID = glCreateShader(GL_VERTEX_SHADER);
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