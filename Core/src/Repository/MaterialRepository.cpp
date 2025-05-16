#include "Repository/MaterialRepository.hpp"

namespace MEngine
{
void PBRMaterialRepository::Update(std::shared_ptr<PBRMaterial> entity)
{
    auto pbrMaterial = std::dynamic_pointer_cast<PBRMaterial>(entity);
    pbrMaterial->SetPBRProperties(pbrMaterial->GetPBRProperties());
}
} // namespace MEngine