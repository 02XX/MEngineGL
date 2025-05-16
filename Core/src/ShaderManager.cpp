#include "Manager/ShaderManager.hpp"
#include "Entity/IShader.hpp"
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
        if (!shader->mVertexShaderPath.empty())
        {
            shader->mVertexShaderID = LoadShader(shader->mVertexShaderID, shader->mVertexShaderPath);
        }
        else
        {
            LogWarn("Vertex shader is empty for shader: {}", shader->mName);
        }
        if (!shader->mFragmentShaderPath.empty())
        {

            shader->mFragmentShaderID = LoadShader(shader->mFragmentShaderID, shader->mFragmentShaderPath);
        }
        else
        {
            LogWarn("Fragment shader is empty for shader: {}", shader->mName);
        }
        if (!shader->mTessControlShaderPath.empty())
        {
            shader->mTessControlShaderID = LoadShader(shader->mTessControlShaderID, shader->mTessControlShaderPath);
        }
        else
        {
            LogWarn("Tessellation shader is empty for shader: {}", shader->mName);
        }
        if (!shader->mGeometryShaderPath.empty())
        {
            shader->mGeometryShaderID = LoadShader(m, shader->mGeometryShaderPath);
        }
        else
        {
            LogWarn("Geometry shader is empty for shader: {}", shader->mName);
        }
        if (!shader->mGeometryShaderPath.empty())
        {
            if (shader->mGeometryShaderID != 0)
            {
                glDeleteShader(shader->mGeometryShaderID);
            }
            shader->mGeometryShaderID = LoadShader(shader->mGeometryShaderPath);
        }
        else
        {
            LogWarn("Geometry shader is empty for shader: {}", shader->mName);
        }
    }
    else
    {
        LogError("Failed to update shader: Shader not found");
    }
}
GLuint ShaderManager::LoadShader(GLuint shaderID, const std::filesystem::path &shaderPath)
{
    if (!std::filesystem::exists(shaderPath))
    {
        LogWarn("Shader file does not exist: {}", shaderPath.string());
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
    LogInfo("Shader compiled successfully: {}", shaderPath.string());
    return shaderID;
}
UUID ShaderManager::Load(const std::filesystem::path &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        LogError("Failed to load shader file: {}", path.string());
        return UUID();
    }
    auto shader = std::make_shared<Shader>();
    nlohmann::json j = nlohmann::json::parse(file);
    *shader = j.get<Shader>();
    auto id = shader->GetID();
    auto it = mEntities.find(id);
    if (it == mEntities.end())
    {
        mEntities[id] = shader;
        Update(id, *shader);
    }
    return id;
}
void ShaderManager::Save(const std::filesystem::path &path, const Shader &entity)
{
    nlohmann::json j;
    j = entity;
    std::ofstream file(path);
    if (!file.is_open())
    {
        LogError("Failed to save shader file: {}", path.string());
        return;
    }
    file << j.dump(4);
    file.close();
}
} // namespace MEngine