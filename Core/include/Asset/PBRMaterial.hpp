#pragma once
#include "Asset/Material.hpp"
#include <glm/vec3.hpp>
namespace MEngine
{
namespace Core
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
  private:
    GLuint UBO;

  public:
    // PBR properties
    PBRParameters Parameters;
    // textures
    UUID AlbedoTextureID = UUID();
    UUID NormalTextureID = UUID();
    UUID ARMTextureID = UUID(); // Ambient, Roughness, Metallic
  public:
    ~PBRMaterial() override = default;
};
} // namespace Core
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Core::PBRParameters>
{
    static void to_json(json &j, const MEngine::Core::PBRParameters &p)
    {
        j["metallic"] = p.metallic;
        j["roughness"] = p.roughness;
        j["albedo"] = {p.albedo.x, p.albedo.y, p.albedo.z};
        j["emissive"] = {p.emissive.x, p.emissive.y, p.emissive.z};
        j["ao"] = p.ao;
        j["emissiveIntensity"] = p.emissiveIntensity;
    }

    static void from_json(const json &j, MEngine::Core::PBRParameters &p)
    {
        p.metallic = j.at("metallic").get<float>();
        p.roughness = j.at("roughness").get<float>();
        auto albedo = j.at("albedo");
        p.albedo = glm::vec3(albedo[0], albedo[1], albedo[2]);
        auto emissive = j.at("emissive");
        p.emissive = glm::vec3(emissive[0], emissive[1], emissive[2]);
        p.ao = j.at("ao").get<float>();
        p.emissiveIntensity = j.at("emissiveIntensity").get<float>();
    }
};

template <> struct adl_serializer<MEngine::Core::PBRMaterial>
{
    static void to_json(json &j, const MEngine::Core::PBRMaterial &material)
    {
        j = static_cast<MEngine::Core::Material>(material);
        j["pbrProperties"] = material.Parameters;
        j["albedoTexture"] = material.AlbedoTextureID.ToString();
        j["normalTexture"] = material.NormalTextureID.ToString();
        j["armTexture"] = material.ARMTextureID.ToString();
    }
    static void from_json(const json &j, MEngine::Core::PBRMaterial &material)
    {
        static_cast<MEngine::Core::Material &>(material) = j.get<MEngine::Core::Material>();
        material.Parameters = j.at("pbrProperties").get<MEngine::Core::PBRParameters>();
        material.AlbedoTextureID = MEngine::Core::UUID(j.at("albedoTexture").get<std::string>());
        material.NormalTextureID = MEngine::Core::UUID(j.at("normalTexture").get<std::string>());
        material.ARMTextureID = MEngine::Core::UUID(j.at("armTexture").get<std::string>());
    }
};
} // namespace nlohmann
