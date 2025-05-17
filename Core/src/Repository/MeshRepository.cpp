#include "Repository/MeshRepository.hpp"

namespace MEngine
{
void MeshRepository::Update(std::shared_ptr<Mesh> entity)
{
    entity->Update();
}
} // namespace MEngine