#pragma once
#include "Entity/Entity.hpp"
#include "Entity/Material.hpp"
#include "Entity/Mesh.hpp"
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
    std::vector<std::shared_ptr<Node>> Children;
    std::vector<int> Meshes; // Mesh indices in the model
    glm::mat4 Transform;
};
class Model : public Entity
{
  public:
    Model();
    std::vector<std::shared_ptr<Mesh>> Meshes;
    std::shared_ptr<Node> RootNode;

    std::filesystem::path ModelPath;
    std::vector<UUID> Materials;
    void Update() override;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Model>
{
    static void to_json(json &j, const MEngine::Model &model)
    {
        j = static_cast<MEngine::Entity>(model);
        j["ModelPath"] = model.ModelPath.string();
        j["Materials"] = model.Materials;
    }

    static void from_json(const json &j, MEngine::Model &model)
    {
        static_cast<MEngine::Entity &>(model) = j;
        model.ModelPath = j.at("ModelPath").get<std::string>();
        model.Materials = j.at("Materials").get<std::vector<MEngine::UUID>>();
    }
};
} // namespace nlohmann