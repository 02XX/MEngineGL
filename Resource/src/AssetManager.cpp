#include "AssetManager.hpp"
#include <queue>
namespace MEngine
{
namespace Core
{
std::queue<Asset *> AssetManager::mInitialAssetQueue{};
std::queue<Asset *> AssetManager::mUpdateAssetQueue{};
std::queue<Asset *> AssetManager::mDeleteAssetQueue{};
std::mutex AssetManager::mInitialAssetMutex{};
std::mutex AssetManager::mUpdateAssetMutex{};
std::mutex AssetManager::mDeleteAssetMutex{};
void AssetManager::AddInitialAsset(Asset *asset)
{
    std::lock_guard<std::mutex> lock(mInitialAssetMutex);
    mInitialAssetQueue.push(asset);
}

void AssetManager::AddUpdateAsset(Asset *asset)
{
    std::lock_guard<std::mutex> lock(mUpdateAssetMutex);
    mUpdateAssetQueue.push(asset);
}

void AssetManager::AddDeleteAsset(Asset *asset)
{
    std::lock_guard<std::mutex> lock(mDeleteAssetMutex);
    mDeleteAssetQueue.push(asset);
}

void AssetManager::ProcessInitialAssets()
{
    std::lock_guard<std::mutex> lock(mInitialAssetMutex);
    while (!mInitialAssetQueue.empty())
    {
        auto asset = mInitialAssetQueue.front();
        mInitialAssetQueue.pop();
    }
}

void AssetManager::ProcessUpdateAssets()
{
    std::lock_guard<std::mutex> lock(mUpdateAssetMutex);
    while (!mUpdateAssetQueue.empty())
    {
        auto asset = mUpdateAssetQueue.front();
        mUpdateAssetQueue.pop();
    }
}

void AssetManager::ProcessDeleteAssets()
{
    std::lock_guard<std::mutex> lock(mDeleteAssetMutex);
    while (!mDeleteAssetQueue.empty())
    {
        auto asset = mDeleteAssetQueue.front();
        mDeleteAssetQueue.pop();
    }
}

} // namespace Core
} // namespace MEngine