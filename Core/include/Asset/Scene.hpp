#pragma once
#include "Asset.hpp"
#include <entt/entt.hpp>
#include <glm/vec3.hpp>

namespace MEngine
{
class Scene final : public Asset
{
  public:
    glm::vec3 GlobalGravity;
    entt::entity MainCamera;
    entt::registry Registry;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Scene>
{
    static void to_json(json &j, const MEngine::Scene &scene)
    {
        j = static_cast<MEngine::Asset>(scene);
        j["GlobalGravity"] = {scene.GlobalGravity.x, scene.GlobalGravity.y, scene.GlobalGravity.z};
        j["MainCamera"] = scene.MainCamera;
    }
    static void from_json(const json &j, MEngine::Scene &scene)
    {
        static_cast<MEngine::Asset &>(scene) = j;
        auto gravity = j.at("GlobalGravity");
        scene.GlobalGravity = glm::vec3(gravity[0], gravity[1], gravity[2]);
        scene.MainCamera = j.at("MainCamera").get<entt::entity>();
    }
};
} // namespace nlohmann