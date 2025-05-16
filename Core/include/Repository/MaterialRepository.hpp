#pragma once
#include "Entity/Material.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Repository/Repository.hpp"
namespace MEngine
{
class PBRMaterialRepository final : public Repository<PBRMaterial>
{
  public:
    PBRMaterialRepository() = default;
    void Update(std::shared_ptr<PBRMaterial> entity) override;
};
template <> struct RepositoryTraits<PBRMaterial>
{
    using RepositoryType = PBRMaterialRepository;
};
} // namespace MEngine