#pragma once
#include "Entity/Material.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Repository/Repository.hpp"
#include "UUID.hpp"
#include <memory>
namespace MEngine
{
class MaterialRepository final : public Repository<Material>
{
  public:
    MaterialRepository()
    {
    }
    void CreateDefault() override
    {
        std::shared_ptr<Material> material = std::make_shared<Material>();
        material->ID = UUID();
        material->Name = "DefaultMaterial";
        material->PipelineID = UUID();
        mEntities[material->ID] = material;
        material->Update();
    }
};
template <> struct RepositoryTraits<Material>
{
    using RepositoryType = MaterialRepository;
};
} // namespace MEngine