#pragma once
#include "Entity/Model.hpp"
#include "Repository/Repository.hpp"

namespace MEngine
{
class ModelRepository : public Repository<Model>
{
  public:
    ModelRepository() {};
    void CreateDefault() override
    {
        std::shared_ptr<Model> model = std::make_shared<Model>();
        model->ID = UUID();
        model->Name = "DefaultModel";
        mEntities[model->ID] = model;
        UpdateAsset(model);
    }
};
} // namespace MEngine