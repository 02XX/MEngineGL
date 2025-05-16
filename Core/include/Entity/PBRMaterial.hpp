#pragma once
#include "Entity/Material.hpp"
#include "Material.hpp"
#include "Math.hpp"
namespace MEngine
{
class PBRMaterial final : public Material
{
  private:
    // PBR properties
    float mMetallic = 0.0f;
    float mRoughness = 0.5f;
    glm::vec3 mColor = glm::vec3(1.0f, 1.0f, 1.0f);
    UUID mAlbedoTexture = UUID();
    UUID mNormalTexture = UUID();
    UUID mARMTexture = UUID(); // Ambient, Roughness, Metallic

    UUID mShaderID = UUID();

  public:
};
} // namespace MEngine
