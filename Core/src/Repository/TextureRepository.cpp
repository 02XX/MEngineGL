#include "Repository/TextureRepository.hpp"

namespace MEngine
{
void Texture2DRepository::Update(std::shared_ptr<Texture2D> entity)
{
    entity->SetImagePath(entity->GetImagePath());
    entity->SetSampler();
}
} // namespace MEngine