#pragma once
#include "Entity/Texture2D.hpp"
#include "Repository/Repository.hpp"
namespace MEngine
{
class Texture2DRepository final : public Repository<Texture2D>
{
  public:
    Texture2DRepository()
    {
    }
    void CreateDefault() override
    {
        std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
        texture->ID = UUID();
        texture->Name = "DefaultTexture";
        texture->ImagePath = mAssetPath / "textures" / "default.png";
        mEntities[texture->ID] = texture;
        texture->Update();
    }
};
template <> struct RepositoryTraits<Texture2D>
{
    using RepositoryType = Texture2DRepository;
};
} // namespace MEngine