#pragma once
#include "Asset/Asset.hpp"
#include <mutex>
#include <queue>
namespace MEngine
{
namespace Core
{
class AssetManager
{
  private:
    static std::queue<Asset *> mInitialAssetQueue;
    static std::queue<Asset *> mUpdateAssetQueue;
    static std::queue<Asset *> mDeleteAssetQueue;
    static std::mutex mInitialAssetMutex;
    static std::mutex mUpdateAssetMutex;
    static std::mutex mDeleteAssetMutex;

  public:
    static void AddInitialAsset(Asset *asset);
    static void AddUpdateAsset(Asset *asset);
    static void AddDeleteAsset(Asset *asset);
    static void ProcessInitialAssets();
    static void ProcessUpdateAssets();
    static void ProcessDeleteAssets();
};
} // namespace Core
} // namespace MEngine