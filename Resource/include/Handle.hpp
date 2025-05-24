#pragma once

#include "Asset/Asset.hpp"
#include "AssetManager.hpp"
#include <atomic>
#include <concepts>
#include <memory>
namespace MEngine
{
namespace Core
{

template <std::derived_from<Asset> TAsset> class Handle
{
  private:
    std::atomic<unsigned int> *mRefCount{nullptr};
    TAsset *mAsset{nullptr};

  public:
    Handle() = default;
    explicit Handle(TAsset *asset) : mAsset(asset), mRefCount(asset ? new std::atomic<unsigned int>(1) : nullptr)
    {
        if (mRefCount)
        {
            AssetManager::AddInitialAsset(mAsset);
        }
    }
    Handle(const Handle &other) : mAsset(other.mAsset), mRefCount(other.mRefCount)
    {
        if (mRefCount)
        {
            mRefCount->fetch_add(1, std::memory_order_relaxed);
        }
    }
    Handle(Handle &&other) noexcept : mAsset(other.mAsset), mRefCount(other.mRefCount)
    {
        other.mAsset = nullptr;
        other.mRefCount = nullptr;
    }
    Handle &operator=(const Handle &other)
    {
        if (this != &other)
        {
            if (mRefCount && mRefCount->fetch_sub(1, std::memory_order_relaxed) == 1)
            {
                delete mRefCount;
                AssetManager::AddDeleteAsset(mAsset);
            }
            mAsset = other.mAsset;
            mRefCount = other.mRefCount;
            if (mRefCount)
            {
                mRefCount->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }
    Handle &operator=(Handle &&other) noexcept
    {
        if (this != &other)
        {
            if (mRefCount && mRefCount->fetch_sub(1, std::memory_order_relaxed) == 1)
            {
                delete mRefCount;
                AssetManager::AddDeleteAsset(mAsset);
            }
            mAsset = other.mAsset;
            mRefCount = other.mRefCount;
            other.mAsset = nullptr;
            other.mRefCount = nullptr;
        }
        return *this;
    }
    ~Handle()
    {
        if (mRefCount && mRefCount->fetch_sub(1, std::memory_order_relaxed) == 1)
        {
            AssetManager::AddDeleteAsset(mAsset);
            delete mRefCount;
        }
    }
    void Reset()
    {
        if (mRefCount && mRefCount->fetch_sub(1, std::memory_order_relaxed) == 1)
        {
            AssetManager::AddDeleteAsset(mAsset);
            delete mRefCount;
        }
        mAsset = nullptr;
        mRefCount = nullptr;
    }
    TAsset *Get() const
    {
        return mAsset;
    }
    unsigned int GetRefCount() const
    {
        return mRefCount ? mRefCount->load(std::memory_order_relaxed) : 0;
    }
    TAsset *operator->() const
    {
        return mAsset;
    }
    TAsset &operator*() const
    {
        return *mAsset;
    }
    explicit operator bool() const
    {
        return mAsset != nullptr;
    }
    bool operator==(const Handle &other) const
    {
        return mAsset == other.mAsset;
    }
    bool operator!=(const Handle &other) const
    {
        return mAsset != other.mAsset;
        std::shared_ptr<TAsset> sharedPtr;
    }
};
template <std::derived_from<Asset> TAsset, typename... Args> Handle<TAsset> MakeHandle(Args &&...args)
{
    return Handle<TAsset>(new TAsset(std::forward<Args>(args)...));
}
} // namespace Core
} // namespace MEngine