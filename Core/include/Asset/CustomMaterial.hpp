#pragma once
#include "Asset/Material.hpp"
#include <glm/vec3.hpp>
namespace MEngine
{

class CustomMaterial final : public Material
{
  public:
    std::vector<std::variant<int, float, glm::vec3>> Parameters;
    std::vector<UUID> Textures;
};

} // namespace MEngine
