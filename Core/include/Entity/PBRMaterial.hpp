#pragma once
#include "Entity/Material.hpp"
#include "Material.hpp"
#include "Math.hpp"
#include "Type.hpp"
namespace MEngine
{
struct PBRParameters
{
    float metallic = 0.0f;
    float roughness = 0.5f;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};
class PBRMaterial final : public Material
{
  public:
    // PBR properties
    PBRParameters Parameters;
    // uniforms
    GLuint UBO;

    // textures
    UUID AlbedoTextureID = UUID();
    UUID NormalTextureID = UUID();
    UUID ARMTextureID = UUID(); // Ambient, Roughness, Metallic
    // shader
    UUID ShaderID = UUID();

    // program
    GLuint programID = 0;

  public:
    PBRMaterial();
    ~PBRMaterial();
    void Update() override;
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
        j["color"] = {p.color.r, p.color.g, p.color.b, p.color.a};
    }

    static void from_json(const json &j, MEngine::PBRParameters &p)
    {
        p.metallic = j.at("metallic").get<float>();
        p.roughness = j.at("roughness").get<float>();
        auto color = j.at("color").get<std::vector<float>>();
        if (color.size() == 4)
        {
            p.color = glm::vec4(color[0], color[1], color[2], color[3]);
        }
        else
        {
            throw std::runtime_error("Invalid color format");
        }
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
