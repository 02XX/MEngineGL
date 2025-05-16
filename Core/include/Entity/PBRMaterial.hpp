#pragma once
#include "Entity/IShader.hpp"
#include "Entity/Material.hpp"
#include "Material.hpp"
#include "Math.hpp"
namespace MEngine
{
class PBRMaterial final : public Material
{
    friend class MaterialManager;

  private:
    // PBR properties
    struct PBRProperties
    {
        float metallic = 0.0f;
        float roughness = 0.5f;
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    } mPBRProperties;

    // uniforms
    GLuint mUBO;
    // textures
    UUID mAlbedoTexture = UUID();
    UUID mNormalTexture = UUID();
    UUID mARMTexture = UUID(); // Ambient, Roughness, Metallic
    // shader
    UUID mShaderID = UUID();

    // program
    GLuint mProgramID = 0;

  public:
    PBRMaterial();
    ~PBRMaterial();

    inline const PBRProperties &GetPBRProperties() const
    {
        return mPBRProperties;
    }
    void SetPBRProperties(const PBRProperties &properties);
    inline void SetAlbedoTexture(const UUID &textureID)
    {
        mAlbedoTexture = textureID;
    }
    inline const UUID &GetAlbedoTexture() const
    {
        return mAlbedoTexture;
    }
    inline void SetNormalTexture(const UUID &textureID)
    {
        mNormalTexture = textureID;
    }
    inline const UUID &GetNormalTexture() const
    {
        return mNormalTexture;
    }
    inline void SetARMTexture(const UUID &textureID)
    {
        mARMTexture = textureID;
    }
    inline const UUID &GetARMTexture() const
    {
        return mARMTexture;
    }
    inline void SetShaderID(const UUID &shaderID)
    {
        mShaderID = shaderID;
    }
    inline const UUID &GetShaderID() const
    {
        return mShaderID;
    }
};
} // namespace MEngine
