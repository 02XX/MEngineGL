#pragma  once
#include "Asset/Material.hpp"
#include "Math.hpp"
namespace MEngine {
struct PhongParameters:
{
  glm::vec3 diffuse = glm::vec3(1.0f);
  glm::vec3 specular = glm::vec3(1.0f);
  glm::vec3 emissive = glm::vec3(0.0f);
  float shininess = 1.0f;
  float emissiveIntensity = 1.0f;
};
class PhongMaterial final: public Material{
    public:
    PhongParameters Parameters;
    UUID DiffuseTextureID = UUID();
    UUID NormalTextureID = UUID();
    UUID EmissiveTextureID = UUID();
    PhongMaterial();
};

} // MEngine

namespace nlohmann
{
    template <> struct adl_serializer<MEngine::PhongMaterial>
    {
        static void to_json(json &j, const MEngine::PhongMaterial &material)
        {
            j = static_cast<MEngine::Material>(material);

        }
        static void from_json(const json &j, MEngine::Material &material)
        {
            static_cast<MEngine::Material&>(material) = j;
        }
    };
}
