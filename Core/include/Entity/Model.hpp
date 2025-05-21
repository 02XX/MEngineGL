#pragma once
#include "Entity/Entity.hpp"
#include "Entity/Mesh.hpp"
#include "UUID.hpp"
#include <memory>
#include <vector>
namespace MEngine
{
struct Node
{
    UUID meshID = UUID();
    UUID materialID = UUID();
    std::vector<std::shared_ptr<Node>> children{};
    std::shared_ptr<Node> parent{};
};
class Model : public Entity
{
  public:
    std::vector<std::shared_ptr<Node>> nodes{};
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Model>
{
    static void to_json(json &j, const MEngine::Model &model)
    {
        j = static_cast<MEngine::Entity>(model);
    }

    static void from_json(const json &j, MEngine::Model &model)
    {
    }
};
} // namespace nlohmann