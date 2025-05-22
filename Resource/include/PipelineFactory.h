//
// Created by 02 on 25-5-22.
//

#pragma once
#include <memory>

#include "Repository/PipelineRepository.hpp"

namespace MEngine {
enum class PipelineType
{
    ForwardPBR,
    ForwardPhong,
    ForwardStandard
};
class PipelineFactory final{
    private:
    std::unordered_map<PipelineType, UUID> mCachedPipeline;
      std::shared_ptr<IRepository<Pipeline>> mPipelineRepository;
    public:
    explicit PipelineFactory(std::shared_ptr<IRepository<Pipeline>> pipelineRepository);
    ~PipelineFactory();
    UUID GetPipeline(PipelineType type);
    void Create(){
        auto forwardPBR = mPipelineRepository->Create();
        forwardPBR->VertexShaderPath = "Assets/Shaders/ForwardPBR.vert";
        forwardPBR->FragmentShaderPath = "Assets/Shaders/ForwardPBR.frag";
        forwardPBR->Update();
        mCachedPipeline[PipelineType::ForwardPBR] = forwardPBR->ID;}
    };

} // MEngine
