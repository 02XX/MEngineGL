#pragma once
#include "Entity/IPipeline.hpp"
#include "Entity/Material.hpp"
#include "Material.hpp"
#include "Math.hpp"
namespace MEngine
{
struct PBRProperties
{
    float metallic = 0.0f;
    float roughness = 0.5f;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
};
class PBRMaterial final : public Material
{
    friend class MaterialManager;
    friend struct nlohmann::adl_serializer<MEngine::PBRMaterial>;

  private:
    // PBR properties
    PBRProperties mPBRProperties;

    // uniforms
    GLuint mUBO;
    // textures
    UUID mAlbedoTextureID = UUID();
    UUID mNormalTextureID = UUID();
    UUID mARMTextureID = UUID(); // Ambient, Roughness, Metallic
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
        mAlbedoTextureID = textureID;
    }
    inline const UUID &GetAlbedoTexture() const
    {
        return mAlbedoTextureID;
    }
    inline void SetNormalTexture(const UUID &textureID)
    {
        mNormalTextureID = textureID;
    }
    inline const UUID &GetNormalTexture() const
    {
        return mNormalTextureID;
    }
    inline void SetARMTexture(const UUID &textureID)
    {
        mARMTextureID = textureID;
    }
    inline const UUID &GetARMTexture() const
    {
        return mARMTextureID;
    }
    inline void SetShaderID(const UUID &shaderID)
    {
        mShaderID = shaderID;
    }
    inline const UUID &GetShaderID() const
    {
        return mShaderID;
    }
    void Update();
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::PBRProperties>
{
    static void to_json(json &j, const MEngine::PBRProperties &p)
    {
        j = json{{"metallic", p.metallic}, {"roughness", p.roughness}, {"color", {p.color.r, p.color.g, p.color.b}}};
    }

    static void from_json(const json &j, MEngine::PBRProperties &p)
    {
        j.at("metallic").get_to(p.metallic);
        j.at("roughness").get_to(p.roughness);
        auto color = j.at("color");
        p.color = glm::vec3(color[0], color[1], color[2]);
    }
};

template <> struct adl_serializer<MEngine::PBRMaterial>
{
    static void to_json(json &j, const MEngine::PBRMaterial &material)
    {
        j = static_cast<MEngine::Material>(material);
        j["pbrProperties"] = material.mPBRProperties;
        j["albedoTexture"] = material.mAlbedoTextureID.ToString();
        j["normalTexture"] = material.mNormalTextureID.ToString();
        j["armTexture"] = material.mARMTextureID.ToString();
    }
    static void from_json(const json &j, MEngine::PBRMaterial &material)
    {
        static_cast<MEngine::Material &>(material) = j.get<MEngine::Material>();
        material.mPBRProperties = j.at("pbrProperties").get<MEngine::PBRProperties>();
        material.mAlbedoTextureID = MEngine::UUID(j.at("albedoTexture").get<std::string>());
        material.mNormalTextureID = MEngine::UUID(j.at("normalTexture").get<std::string>());
        material.mAlbedoTextureID = MEngine::UUID(j.at("armTexture").get<std::string>());
    }
};
} // namespace nlohmann
