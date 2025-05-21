#pragma once
#include "Entity/Mesh.hpp"
#include "Repository/Repository.hpp"
namespace MEngine
{
class MeshRepository : public Repository<Mesh>
{
  public:
    MeshRepository()
    {

    };
    void CreateDefault() override
    {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        mesh->ID = UUID();
        mesh->Name = "DefaultMesh";
        mEntities[mesh->ID] = mesh;
        mesh->Update();
    }
};
template <> struct RepositoryTraits<Mesh>
{
    using RepositoryType = MeshRepository;
};
} // namespace MEngine