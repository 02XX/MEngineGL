#include "AssetManager.hpp"
#include "Logger.hpp"
namespace MEngine
{
namespace Function
{
void AssetManager::ProcessInitialQueuedAssets()
{
}
void AssetManager::ProcessUpdatedAssets()
{
}
void AssetManager::ProcessDeletedAssets()
{
}
void AssetManager::CheckAssert()
{
    for (auto &&[id, asset] : mCachedAssets)
    {
        if (asset.use_count() == 1)
        {
            AddDeleteQueue(id);
            mCachedAssets.erase(id);
            LogTrace("Asset with ID {} has no references, marked for deletion.", id.ToString());
        }
    }
}
void AssetManager::AddInitialQueue(const UUID &id)
{
    mInitialQueue.push(id);
}
void AssetManager::AddUpdatedQueue(const UUID &id)
{
    mUpdatedQueue.push(id);
}
void AssetManager::AddDeleteQueue(const UUID &id)
{
    mDeletedQueue.push(id);
}
} // namespace Function
} // namespace MEngine
