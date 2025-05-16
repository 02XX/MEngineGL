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
    float mMetallic = 0.0f;
    float mRoughness = 0.5f;
    glm::vec3 mColor = glm::vec3(1.0f, 1.0f, 1.0f);

    // uniforms
    GLuint mUBO;
    // textures
    UUID mAlbedoTexture = UUID();
    UUID mNormalTexture = UUID();
    UUID mARMTexture = UUID(); // Ambient, Roughness, Metallic
    // shader
    UUID mShaderID = UUID();

  public:
    PBRMaterial() = default;

    inline void SetMetallic(float metallic)
    {
        mMetallic = metallic;
    }
    inline float GetMetallic() const
    {
        return mMetallic;
    }

    inline void SetRoughness(float roughness)
    {
        mRoughness = roughness;
    }
    inline float GetRoughness() const
    {
        return mRoughness;
    }

    inline void SetColor(const glm::vec3 &color)
    {
        mColor = color;
    }
    inline const glm::vec3 &GetColor() const
    {
        return mColor;
    }

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
