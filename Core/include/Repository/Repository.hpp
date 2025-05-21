#pragma once
#include "Entity/Entity.hpp"
#include "Entity/Folder.hpp"
#include "Logger.hpp"
#include "Repository/IRepository.hpp"
#include <filesystem>
#include <typeindex>
#include <unordered_map>

namespace MEngine
{
template <typename TEntity>
    requires std::derived_from<TEntity, Entity>
class Repository : public IRepository<TEntity>
{
  protected:
    std::unordered_map<UUID, std::shared_ptr<TEntity>> mEntities;
    std::unordered_map<std::filesystem::path, UUID> mCachedAssets;
    std::filesystem::path mAssetPath = std::filesystem::current_path() / "Assets";

  public:
    Repository() = default;
    virtual ~Repository() = default;
    virtual std::shared_ptr<TEntity> Create() override
    {
        auto entity = std::make_shared<TEntity>();
        mEntities[entity->ID] = entity;
        return entity;
    }
    std::shared_ptr<TEntity> LoadAsset(const std::filesystem::path &path) override
    {
        auto entity = std::make_shared<TEntity>();
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("path does not exist: " + path.string());
        }
        entity->SourcePath = path;
        entity->Name = path.filename().string();
        auto cachedAsset = mCachedAssets.find(path);
        if (cachedAsset != mCachedAssets.end())
        {
            LogTrace("Find cached asset: {} in {}", cachedAsset->second.ToString(), path.string());
            return GetAsset(cachedAsset->second);
        }
        if (std::filesystem::is_directory(path) && typeid(TEntity) == typeid(Folder))
        {
            mEntities[entity->ID] = entity;
            mCachedAssets[path] = entity->ID;
            return entity;
        }
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + path.string());
        }
        json j = json::parse(file);
        j.get_to<TEntity>(*entity);
        entity->SourcePath = path;
        if (mEntities.contains(entity->ID))
        {
            LogWarn("Entity with ID {} already exists.", entity->ID.ToString());
        }
        mEntities[entity->ID] = entity;
        mCachedAssets[path] = entity->ID;
        UpdateAsset(entity);
        LogInfo("Loaded asset: {} from {}", entity->ID.ToString(), path.string());
        return entity;
    }
    void SaveAsset(const UUID &id, const std::filesystem::path &path) override
    {
        auto entity = GetAsset(id);
        entity->SourcePath = path;
        std::ofstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to create file: " + path.string());
        }
        json j;
        j = *entity;
        file << j.dump(4);
        file.close();
        LogInfo("Saved asset: {} to {}", entity->ID.ToString(), path.string());
    }
    virtual std::shared_ptr<TEntity> GetAsset(const UUID &id) override
    {
        auto it = mEntities.find(id);
        if (it != mEntities.end())
        {
            return it->second;
        }
        else
        {
            LogError("Entity with ID {} not found.", id.ToString());
            return nullptr;
        }
    }
    virtual void UpdateAsset(std::shared_ptr<TEntity> entity) override
    {
        auto it = mEntities.find(entity->ID);
        if (it == mEntities.end())
        {
            LogError("Entity with ID {} not found.", entity->ID.ToString());
            return;
        }
        entity->Update();
        if (!entity->SourcePath.empty())
        {
            SaveAsset(entity->ID, entity->SourcePath);
        }
        LogInfo("Updated asset: {}", entity->ID.ToString());
    }
    virtual void DeleteAsset(const UUID &id) override
    {
        auto it = mEntities.find(id);
        if (it != mEntities.end())
        {
            std::filesystem::remove(it->second->SourcePath);
            mEntities.erase(it);
        }
    }
};
template <typename T> struct RepositoryTraits
{
    using RepositoryType = Repository<T>;
};
} // namespace MEngine