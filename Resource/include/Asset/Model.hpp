#pragma once
#include "Asset/Asset.hpp"
#include "UUID.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>

namespace MEngine
{
namespace Core
{
struct Node
{
    std::string Name;
    std::vector<Node *> Children;
    std::vector<int> MesheIndices; // Mesh indices in the model
    glm::mat4 LocalTransform;
};
class Model : public Asset
{
  public:
    Model();
    std::vector<UUID> Meshes;
    std::vector<UUID> Materials;
    std::unique_ptr<Node> RootNode;
};
} // namespace Core
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Core::Model>
{
    static void to_json(json &j, const MEngine::Core::Model &model)
    {
        j = static_cast<MEngine::Core::Asset>(model);
        j["Meshes"] = model.Meshes;
        j["Materials"] = model.Materials;
    }

    static void from_json(const json &j, MEngine::Core::Model &model)
    {
        static_cast<MEngine::Core::Asset &>(model) = j;
        model.Meshes = j.at("Meshes").get<std::vector<MEngine::Core::UUID>>();
        model.Materials = j.at("Materials").get<std::vector<MEngine::Core::UUID>>();
    }
};
} // namespace nlohmann