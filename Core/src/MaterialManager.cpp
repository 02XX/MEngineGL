#include "Manager/MaterialManager.hpp"

namespace MEngine
{
std::shared_ptr<Material> MaterialManager::Create()
{
    auto material = std::make_shared<Material>();
    UUID id = UUIDGenerator()();
    material->mID = id;
    material->mName = "Material_" + id.ToString();
    mEntities[id] = material;
    return material;
}
void MaterialManager::Update(const UUID &id, const Material &entity)
{
    switch (entity.GetMaterialType())
    {
    case MaterialType::PBR:
        // Update PBR material
        
    case MaterialType::Phong:
        break;
    }
}
UUID MaterialManager::Load(const std::filesystem::path &path)
{
}
void MaterialManager::Save(const std::filesystem::path &path, const Material &entity)
{
}
} // namespace MEngine