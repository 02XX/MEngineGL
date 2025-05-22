#pragma once
#include "Entity/Pipeline.hpp"
#include "Repository/Repository.hpp"
namespace MEngine
{
class PipelineRepository final : public Repository<Pipeline>
{
  public:
    PipelineRepository()
    {
    }
    void CreateDefault() override
    {
        std::shared_ptr<Pipeline> pipeline = std::make_shared<Pipeline>();
        pipeline->ID = UUID();
        pipeline->Name = "DefaultPipeline";
        pipeline->VertexShaderPath = mAssetPath / "shaders" / "default.vert";
        pipeline->FragmentShaderPath = mAssetPath / "shaders" / "default.frag";
        mEntities[pipeline->ID] = pipeline;
        pipeline->Update();
    }
};
template <> struct RepositoryTraits<Pipeline>
{
    using RepositoryType = PipelineRepository;
};
} // namespace MEngine