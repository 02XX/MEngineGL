#include "Repository/MeshRepository.hpp"

namespace MEngine
{
void MeshRepository::Update(std::shared_ptr<Mesh> entity)
{
    entity->SetVertexBuffer(entity->GetVertices());
    entity->SetIndexBuffer(entity->GetIndices());
}
} // namespace MEngine