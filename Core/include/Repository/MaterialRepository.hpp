#pragma once
#include "Entity/Material.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Repository/Repository.hpp"
namespace MEngine
{
class MaterialRepository final : public Repository<Material>
{
  public:
    MaterialRepository() = default;
    void Update(std::shared_ptr<Material> entity) override;
};
template <> struct RepositoryTraits<PBRMaterial>
{
    using RepositoryType = MaterialRepository;
};
} // namespace MEngine