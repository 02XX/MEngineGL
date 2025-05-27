#pragma once
#include "Asset/Asset.hpp"
#include "UUID.hpp"
#include <memory>
#include <queue>
#include <typeindex>
#include <unordered_map>
using namespace MEngine::Core;
namespace MEngine
{

namespace Function
{

class IAssetManager
{
  public:
    virtual ~IAssetManager() = 0;
    virtual std::shared_ptr<Asset> GetAssetByID(const UUID &id) = 0;
};

class AssetManager : public IAssetManager, public std::enable_shared_from_this<AssetManager>
{

  private:
    std::queue<UUID> mInitialQueue;
    std::queue<UUID> mUpdatedQueue;
    std::queue<UUID> mDeletedQueue;
    std::unordered_map<UUID, std::shared_ptr<Asset>> mCachedAssets;

  public:
    virtual ~AssetManager() = 0;
    template <std::derived_from<Asset> TAsset> std::shared_ptr<Asset> GetAssetByID(const UUID &id)
    {
        if (auto it = mCachedAssets.find(id); it != mCachedAssets.end())
        {
            auto asset = std::dynamic_pointer_cast<TAsset>(it->second);
            return asset;
        }
        else
        {
            // 如果缓存中没有，尝试从数据库中获取，并加载
            auto asset = std::dynamic_pointer_cast<TAsset>(GetAssetByIDImpl(typeid(TAsset), id));
            mInitialQueue.push(id);
            mCachedAssets[id] = asset;
        }
    }
    void ProcessInitialQueuedAssets();
    void ProcessUpdatedAssets();
    void ProcessDeletedAssets();

    void CheckAssert();
    void AddInitialQueue(const UUID &id);
    void AddUpdatedQueue(const UUID &id);
    void AddDeleteQueue(const UUID &id);

  protected:
    virtual std::shared_ptr<Asset> GetAssetByIDImpl(std::type_index type, const UUID &id) = 0;
};
} // namespace Function
} // namespace MEngine