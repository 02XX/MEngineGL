#include "Entity/PBRMaterial.hpp"
#include "Entity/IShader.hpp"

namespace MEngine
{
PBRMaterial::PBRMaterial()
{
    mName = "PBRMaterial";
    mMaterialType = MaterialType::PBR;
    glCreateBuffers(1, &mUBO);
    glNamedBufferStorage(mUBO, sizeof(PBRProperties), nullptr, GL_DYNAMIC_STORAGE_BIT);
}
PBRMaterial::~PBRMaterial()
{
    glDeleteBuffers(1, &mUBO);
}
void PBRMaterial::SetPBRProperties(const PBRProperties &properties)
{
    mPBRProperties = properties;
    glNamedBufferSubData(mUBO, 0, sizeof(PBRProperties), &mPBRProperties);
}
} // namespace MEngine