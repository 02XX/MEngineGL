#pragma once
#include "Model.hpp"
namespace MEngine
{
namespace Core
{
class Prefab final : public Asset
{
  public:
    std::vector<UUID> Meshes;
    std::vector<UUID> Materials;
    std::unique_ptr<Node> RootNode;
};
} // namespace Core
} // namespace MEngine
namespace nlohmann
{
template <> struct adl_serializer<MEngine::Core::Prefab>
{
    static void to_json(json &j, const MEngine::Core::Prefab &prefab)
    {
        j = static_cast<MEngine::Core::Asset>(prefab);
        j["Meshes"] = prefab.Meshes;
        j["Materials"] = prefab.Materials;
    }

    static void from_json(const json &j, MEngine::Core::Prefab &prefab)
    {
        static_cast<MEngine::Core::Asset &>(prefab) = j;
        prefab.Meshes = j.at("Meshes").get<std::vector<MEngine::Core::UUID>>();
        prefab.Materials = j.at("Materials").get<std::vector<MEngine::Core::UUID>>();
    }
};
} // namespace nlohmann
