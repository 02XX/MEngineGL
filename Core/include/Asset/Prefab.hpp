#pragma once
#include "Model.hpp"
namespace MEngine
{

class Prefab final : public Asset
{
  public:
    std::vector<UUID> Meshes;
    std::vector<UUID> Materials;
    std::unique_ptr<Node> RootNode;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Prefab>
{
    static void to_json(json &j, const MEngine::Prefab &prefab)
    {
        j = static_cast<MEngine::Asset>(prefab);
        j["Meshes"] = prefab.Meshes;
        j["Materials"] = prefab.Materials;
    }

    static void from_json(const json &j, MEngine::Prefab &prefab)
    {
        static_cast<MEngine::Asset &>(prefab) = j;
        prefab.Meshes = j.at("Meshes").get<std::vector<MEngine::UUID>>();
        prefab.Materials = j.at("Materials").get<std::vector<MEngine::UUID>>();
    }
};
} // namespace nlohmann
