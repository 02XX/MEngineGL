#pragma once
#include "Asset/Material.hpp"
#include <glm/vec3.hpp>
namespace MEngine
{
namespace Core
{
struct PhongParameters
{
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    glm::vec3 emissive = glm::vec3(0.0f);
    float shininess = 1.0f;
    float emissiveIntensity = 1.0f;
};
class PhongMaterial final : public Material
{
  private:
    GLuint UBO;

  public:
    PhongParameters Parameters;
    UUID DiffuseTextureID = UUID();
    UUID NormalTextureID = UUID();
    UUID EmissiveTextureID = UUID();
    ~PhongMaterial() override = default;
};
} // namespace Core
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Core::PhongParameters>
{
    static void to_json(json &j, const MEngine::Core::PhongParameters &p)
    {
        j["diffuse"] = {p.diffuse.x, p.diffuse.y, p.diffuse.z};
        j["specular"] = {p.specular.x, p.specular.y, p.specular.z};
        j["emissive"] = {p.emissive.x, p.emissive.y, p.emissive.z};
        j["shininess"] = p.shininess;
        j["emissiveIntensity"] = p.emissiveIntensity;
    }
    static void from_json(const json &j, MEngine::Core::PhongParameters &p)
    {
        auto diffuse = j.at("diffuse");
        p.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
        auto specular = j.at("specular");
        p.specular = glm::vec3(specular[0], specular[1], specular[2]);
        auto emissive = j.at("emissive");
        p.emissive = glm::vec3(emissive[0], emissive[1], emissive[2]);
        p.shininess = j.at("shininess").get<float>();
        p.emissiveIntensity = j.at("emissiveIntensity").get<float>();
    }
};
template <> struct adl_serializer<MEngine::Core::PhongMaterial>
{
    static void to_json(json &j, const MEngine::Core::PhongMaterial &material)
    {
        j = static_cast<MEngine::Core::Material>(material);
        j["DiffuseTextureID"] = material.DiffuseTextureID;
        j["NormalTextureID"] = material.NormalTextureID;
        j["EmissiveTextureID"] = material.EmissiveTextureID;
        j["Parameters"] = material.Parameters;
    }
    static void from_json(const json &j, MEngine::Core::PhongMaterial &material)
    {
        static_cast<MEngine::Core::Material &>(material) = j;
        material.DiffuseTextureID = j.at("DiffuseTextureID").get<MEngine::Core::UUID>();
        material.NormalTextureID = j.at("NormalTextureID").get<MEngine::Core::UUID>();
        material.EmissiveTextureID = j.at("EmissiveTextureID").get<MEngine::Core::UUID>();
        material.Parameters = j.at("Parameters").get<MEngine::Core::PhongParameters>();
    }
};
} // namespace nlohmann
