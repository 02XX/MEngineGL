#pragma once
#include "Entity/StandardMaterial.hpp"
#include "Repository/Repository.hpp"
#include "UUID.hpp"
#include <memory>
namespace MEngine
{
class StandardMaterialRepository final : public Repository<StandardMaterial>
{
  public:
    StandardMaterialRepository();
    void CreateDefault() override
    {
        auto standardMaterial = std::make_shared<StandardMaterial>();
        standardMaterial->ID = UUID();
        standardMaterial->Name = "DefaultStandardMaterial";
        standardMaterial->PipelineID = UUID();
        mEntities[standardMaterial->ID] = standardMaterial;
        standardMaterial->Update();
    }
    
};



template <> struct RepositoryTraits<Material>
{
    using RepositoryType = StandardMaterialRepository;
};
} // namespace MEngine