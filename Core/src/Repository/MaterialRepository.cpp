#include "Repository/MaterialRepository.hpp"

namespace MEngine
{
void MaterialRepository::Update(std::shared_ptr<Material> entity)
{
    switch (entity->GetMaterialType())
    {
    case MaterialType::PBR: {
        auto pbrMaterial = std::dynamic_pointer_cast<PBRMaterial>(entity);
        pbrMaterial->SetPBRProperties(pbrMaterial->GetPBRProperties());
    }
    break;
    case MaterialType::Phong:
        break;
    }
}
} // namespace MEngine