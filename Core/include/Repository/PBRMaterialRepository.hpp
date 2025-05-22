//
// Created by 02 on 25-5-22.
//

#pragma once
#include "Repository/Repository.hpp"
#include "Entity/PBRMaterial.hpp"
namespace MEngine {

class PBRMaterialRepository : public Repository<PBRMaterial>{
    public:
    void CreateDefault() override
    {
        auto material = std::make_shared<PBRMaterial>();
        material->ID = UUID();
        material->Name = "DefaultPBRMaterial";
        material->PipelineID = UUID();
        mEntities[material->ID] = material;
        material->Update();
    }
};

} // MEngine


