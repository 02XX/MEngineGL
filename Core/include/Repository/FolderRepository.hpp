#pragma once
#include "Entity/Folder.hpp"
#include "Repository/Repository.hpp"

namespace MEngine
{
class FolderRepository : public Repository<Folder>
{
  public:
    FolderRepository() {};
    void CreateDefault() override
    {
    }
};
} // namespace MEngine