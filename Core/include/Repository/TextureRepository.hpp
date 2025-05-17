#pragma once
#include "Entity/Shader.hpp"
#include "Entity/Texture2D.hpp"
#include "Repository/Repository.hpp"
namespace MEngine
{
class Texture2DRepository final : public Repository<Texture2D>
{
  public:
    Texture2DRepository() = default;
    void Update(std::shared_ptr<Texture2D> entity) override;
};
template <> struct RepositoryTraits<Texture2D>
{
    using RepositoryType = Texture2DRepository;
};
} // namespace MEngine