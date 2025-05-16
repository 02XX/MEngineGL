#pragma once
#include "Entity/Shader.hpp"
#include "Repository/Repository.hpp"
namespace MEngine
{
class ShaderRepository final : public Repository<Shader>
{
  public:
    ShaderRepository() = default;
    void Update(std::shared_ptr<Shader> entity) override;
};
template <> struct RepositoryTraits<Shader>
{
    using RepositoryType = ShaderRepository;
};
} // namespace MEngine