#pragma once
#include "Asset/Asset.hpp"
#include "Asset/Material.hpp"
#include "Asset/Mesh.hpp"
#include "UUID.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <memory>
#include <vector>

namespace MEngine
{
struct Node
{
    std::string Name;
    std::vector<Node*> Children;
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
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Model>
{
    static void to_json(json &j, const MEngine::Model &model)
    {
        j = static_cast<MEngine::Asset>(model);
        j["ModelPath"] = model.ModelPath.string();
        j["Materials"] = model.Materials;
    }

    static void from_json(const json &j, MEngine::Model &model)
    {
        static_cast<MEngine::Asset &>(model) = j;
        model.ModelPath = j.at("ModelPath").get<std::string>();
        model.Materials = j.at("Materials").get<std::vector<MEngine::UUID>>();
    }
};
} // namespace nlohmann