#include "Entity/PBRMaterial.hpp"

namespace MEngine
{
PBRMaterial::PBRMaterial()
{
    Name = "PBRMaterial";
    MaterialType = MaterialType::PBR;
    glCreateBuffers(1, &UBO);
    glNamedBufferStorage(UBO, sizeof(PBRParameters), nullptr, GL_DYNAMIC_STORAGE_BIT);
}
PBRMaterial::~PBRMaterial()
{
    glDeleteBuffers(1, &UBO);
}
void PBRMaterial::Update()
{
    auto parameters = Parameters.Get();
    glNamedBufferSubData(UBO, 0, sizeof(PBRParameters), &parameters);
}
} // namespace MEngine