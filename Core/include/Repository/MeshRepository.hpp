#pragma once
#include "Entity/Mesh.hpp"
#include "Repository/Repository.hpp"
namespace MEngine
{
class MeshRepository : public Repository<Mesh>
{
  public:
    MeshRepository() = default;
    void Update(std::shared_ptr<Mesh> entity) override;
};
template <> struct RepositoryTraits<Mesh>
{
    using RepositoryType = MeshRepository;
};
} // namespace MEngine