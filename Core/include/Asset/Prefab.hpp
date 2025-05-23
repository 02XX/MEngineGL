#pragma once
#include "Model.hpp"
namespace MEngine {

class Prefab final : public Asset
{
public:
    std::vector<UUID> Meshes;
    std::vector<UUID> Materials;
    std::unique_ptr<Node> RootNode;
};
} // MEngine


