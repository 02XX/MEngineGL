//
// Created by 02 on 25-5-22.
//

#include "PipelineFactory.h"

namespace MEngine {
    PipelineFactory::PipelineFactory(std::shared_ptr<IRepository<Pipeline>> pipelineRepository)
        :mPipelineRepository(pipelineRepository)
    {


        // auto forwardStandard = mPipelineRepository->Create();
        // forwardStandard->VertexShaderPath = "Shaders/ForwardStandard.vs";
        // forwardStandard->FragmentShaderPath = "Shaders/ForwardStandard.fs";
        // forwardStandard->Update();
    }

    PipelineFactory::~PipelineFactory()
    {
    }
    UUID PipelineFactory::GetPipeline(PipelineType type)
    {
        if (const auto it = mCachedPipeline.find(type); it != mCachedPipeline.end())
        {
            return it->second;
        }
        return UUID();
    }
} // MEngine