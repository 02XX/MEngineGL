#pragma once
#include "Entity/Pipeline.hpp"
#include "Repository/Repository.hpp"
namespace MEngine
{
class PipelineRepository final : public Repository<Pipeline>
{
  public:
    PipelineRepository() = default;
    void Update(std::shared_ptr<Pipeline> entity) override;
};
template <> struct RepositoryTraits<Pipeline>
{
    using RepositoryType = PipelineRepository;
};
} // namespace MEngine