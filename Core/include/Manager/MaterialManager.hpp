#pragma once
#include "Entity/Material.hpp"
#include "Manager/Manager.hpp"
namespace MEngine
{
class MaterialManager final : public Manager<Material>
{
  public:
    std::shared_ptr<Material> Create() override;
    void Update(const UUID &id, const Material &entity) override;
    UUID Load(const std::filesystem::path &path) override;
    void Save(const std::filesystem::path &path, const Material &entity) override;
};
} // namespace MEngine