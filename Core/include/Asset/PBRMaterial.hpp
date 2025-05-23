#pragma once
#include "Asset/Material.hpp"
#include "Material.hpp"
#include "Math.hpp"
#include "Type.hpp"
namespace MEngine
{
struct PBRParameters
{
    glm::vec3 albedo = glm::vec3(1.0f);
    glm::vec3 emissive = glm::vec3(0.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 0.0f;
    float emissiveIntensity = 0.0f;
};
class PBRMaterial final : public Material
{
  public:
    // PBR properties
    PBRParameters Parameters;
    // textures
    UUID AlbedoTextureID = UUID();
    UUID NormalTextureID = UUID();
    UUID ARMTextureID = UUID(); // Ambient, Roughness, Metallic
  public:
    PBRMaterial();
    ~PBRMaterial() override;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::PBRParameters>
{
    static void to_json(json &j, const MEngine::PBRParameters &p)
    {
        j["metallic"] = p.metallic;
        j["roughness"] = p.roughness;

    }

    static void from_json(const json &j, MEngine::PBRParameters &p)
    {
        p.metallic = j.at("metallic").get<float>();
        p.roughness = j.at("roughness").get<float>();

    }
};

template <> struct adl_serializer<MEngine::PBRMaterial>
{
    static void to_json(json &j, const MEngine::PBRMaterial &material)
    {
        j = static_cast<MEngine::Material>(material);
        j["pbrProperties"] = material.Parameters;
        j["albedoTexture"] = material.AlbedoTextureID.ToString();
        j["normalTexture"] = material.NormalTextureID.ToString();
        j["armTexture"] = material.ARMTextureID.ToString();
    }
    static void from_json(const json &j, MEngine::PBRMaterial &material)
    {
        static_cast<MEngine::Material &>(material) = j.get<MEngine::Material>();
        material.Parameters = j.at("pbrProperties").get<MEngine::PBRParameters>();
        material.AlbedoTextureID = MEngine::UUID(j.at("albedoTexture").get<std::string>());
        material.NormalTextureID = MEngine::UUID(j.at("normalTexture").get<std::string>());
        material.ARMTextureID = MEngine::UUID(j.at("armTexture").get<std::string>());
    }
};
} // namespace nlohmann
