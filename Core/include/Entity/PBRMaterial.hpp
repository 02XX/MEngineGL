#pragma once
#include "Entity/IShader.hpp"
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

  private:
    // PBR properties
    PBRProperties mPBRProperties;

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
        j["pbrProperties"] = material.GetPBRProperties();
        j["albedoTexture"] = material.GetAlbedoTexture().ToString();
        j["normalTexture"] = material.GetNormalTexture().ToString();
        j["armTexture"] = material.GetARMTexture().ToString();
    }
    static void from_json(const json &j, MEngine::PBRMaterial &material)
    {
        static_cast<MEngine::Material &>(material) = j.get<MEngine::Material>();
        material.SetPBRProperties(j.at("pbrProperties").get<MEngine::PBRProperties>());
        material.SetAlbedoTexture(MEngine::UUID(j.at("albedoTexture").get<std::string>()));
        material.SetNormalTexture(MEngine::UUID(j.at("normalTexture").get<std::string>()));
        material.SetARMTexture(MEngine::UUID(j.at("armTexture").get<std::string>()));
    }
};
} // namespace nlohmann
