#pragma once

#include <concepts>
#include <filesystem>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "Entity/Entity.hpp"
#include "Entity/IEntity.hpp"
#include "Entity/Material.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Entity/Pipeline.hpp"
#include "Entity/Texture2D.hpp"
#include "Logger.hpp"
#include "Repository/IRepository.hpp"
#include "Repository/MaterialRepository.hpp"
#include "Repository/PipelineRepository.hpp"
#include "Repository/Repository.hpp"
#include "Repository/TextureRepository.hpp"
#include "UUID.hpp"

using json = nlohmann::json;
namespace MEngine
{
class ResourceManager
{
  private:
    std::unordered_map<UUID, std::shared_ptr<IEntity>> mEntities;
    const std::unordered_map<std::type_index, std::string> mTypeExtensions = {
        {typeid(Pipeline), ".shader"},
        {typeid(PBRMaterial), ".pbrmaterial"},
        {typeid(Texture2D), ".tex2d"},
        {typeid(Texture2D), ".texture"},
        // {typeid(Material), ".material"}, {typeid(Animation), ".animation"}, {typeid(Model), ".model"},
        // {typeid(Audio), ".audio"},       {typeid(Scene), ".scene"},
    };
    const std::unordered_map<std::string, std::type_index> mExtensionTypes = {
        {".shader", typeid(Pipeline)}, {".pbrmaterial", typeid(PBRMaterial)}, {".tex2d", typeid(Texture2D)}
        // {".texture", typeid(Texture)},     {".mesh", typeid(Mesh)},
        // {".material", typeid(Material)},   {".animation", typeid(Animation)},
        // {".model", typeid(Model)},         {".audio", typeid(Audio)},
        // {".scene", typeid(Scene)},
    };
    const std::unordered_map<std::string, std::type_index> mRawAssets = {
        {".png", typeid(Texture2D)}, {".jpg", typeid(Texture2D)}, {".jpeg", typeid(Texture2D)},
        {".bmp", typeid(Texture2D)}, {".tga", typeid(Texture2D)}, {".hdr", typeid(Texture2D)},
        {".exr", typeid(Texture2D)},
    };

  public:
    bool IsRawAsset(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("File does not exist: " + path.string());
        }
        std::string extension = path.extension().string();
        auto it = mRawAssets.find(extension);
        return it != mRawAssets.end();
    }
    void CreateAssetForRaw(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("File does not exist: " + path.string());
        }
        std::string extension = path.extension().string();
        auto typeIndex = mRawAssets.find(extension);
        if (typeIndex != mRawAssets.end())
        {
            auto assetPath = path;
            assetPath.replace_extension(mTypeExtensions.at(typeIndex->second));
            if (std::filesystem::exists(assetPath))
            {
                LogTrace("Asset already exists: {}", assetPath.string());
                return;
            }
            else
            {
                std::shared_ptr<IEntity> entity = nullptr;
                if (typeIndex->second == typeid(Texture2D))
                {
                    entity = std::make_shared<Texture2D>();
                    auto texture = std::dynamic_pointer_cast<Texture2D>(entity);
                    texture->SetImagePath(path);
                    Serialize<Texture2D>(assetPath, texture);
                    LogTrace("Create texture2D asset: {}", assetPath.string());
                }
                else
                {
                    LogWarn("Not implement file type {}", mTypeExtensions.at(typeIndex->second));
                }
                if (entity)
                {
                    entity->SetPath(assetPath);
                }
            }
        }
        else
        {
            LogWarn("No Raw to Asset mappiping, {}", extension);
            return;
        }
    }
    /**
     * @brief 加载资源
     *
     */
    UUID LoadAsset(const std::filesystem::path &path)
    {
        // 检查文件是否存在
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("File does not exist: " + path.string());
        }
        std::string extension = path.extension().string();
        // 检查扩展名是否支持
        auto typeIndex = mExtensionTypes.find(extension);
        if (typeIndex == mExtensionTypes.end())
        {
            LogTrace("Unsupported file type: {}", extension);
            return UUID();
        }

        auto loadEntity = [&]<typename TEntity>(const std::type_index &typeIdx) -> UUID
            requires std::derived_from<TEntity, IEntity>
        {
            auto entity = std::make_shared<TEntity>();
            Deserialize<TEntity>(path, entity);
            // 检查 ID 是否已存在
            if (mEntities.contains(entity->GetID()))
            {
                LogWarn("Entity with ID {} already exists.", entity->GetID().ToString());
            }
            // 更新相应的存储库
            using repoType = typename RepositoryTraits<TEntity>::RepositoryType;
            static repoType repository;
            repository.Update(entity);
            // 存储实体并返回 ID
            mEntities[entity->GetID()] = entity;
            LogInfo("Loaded asset: {}", path.string());
            return entity->GetID();
        };
        // 根据类型索引分发加载逻辑
        const auto &typeIdx = typeIndex->second;
        if (typeIndex != mExtensionTypes.end())
        {
            if (typeIdx == typeid(Pipeline))
            {
                return loadEntity.template operator()<Pipeline>(typeIdx);
            }
            else if (typeIdx == typeid(PBRMaterial))
            {
                return loadEntity.template operator()<PBRMaterial>(typeIdx);
            }
            else if (typeIdx == typeid(Texture2D))
            {
                return loadEntity.template operator()<Texture2D>(typeIdx);
            }
            else
            {
                LogWarn("Not implement file type {}", mTypeExtensions.at(typeIndex->second));
                return UUID();
            }
        }
        else
        {
            LogTrace("Unsupported file type: {}", extension);
            return UUID();
        }
    }

    /**
     * @brief 创建新的资源
     * @tparam TEntity 实体类型，需继承自 IEntity
     * @return UUID 资源的唯一标识
     * @throws std::runtime_error 如果文件创建失败
     */
    template <typename TEntity>
        requires std::derived_from<TEntity, IEntity>
    UUID CreateAsset(const std::filesystem::path &path, std::string name = "New Asset")
    {
        if (!std::filesystem::exists(path))
        {
            LogError("Project path does not exist: {}", path.string());
            throw std::runtime_error("path does not exist: " + path.string());
        }
        using RepositoryType = typename RepositoryTraits<TEntity>::RepositoryType;
        static RepositoryType repository;
        auto entity = repository.Create();
        mEntities[entity->GetID()] = entity;
        std::filesystem::path sourcePath = GenerateUniquePath<TEntity>(path, name);
        entity->SetPath(sourcePath);
        Serialize(sourcePath, entity);
        return entity->GetID();
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, IEntity>
    void Serialize(std::filesystem::path path, std::shared_ptr<TEntity> target)
    {
        std::ofstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to create file: " + path.string());
        }
        json j;
        j = *target;
        file << j.dump(4);
        file.close();
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, IEntity>
    void Deserialize(std::filesystem::path path, std::shared_ptr<TEntity> target)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("path does not exist: " + path.string());
        }
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + path.string());
        }
        json j = json::parse(file);
        j.get_to<TEntity>(*target);
    }
    /**
     * @brief 获取资源
     * @param id 资源的唯一标识
     * @return std::shared_ptr<TEntity> 资源实体
     */
    template <typename TEntity>
        requires std::derived_from<TEntity, IEntity>
    std::shared_ptr<TEntity> GetAsset(const UUID &id)
    {
        auto it = mEntities.find(id);
        if (it != mEntities.end())
        {
            auto entity = std::dynamic_pointer_cast<TEntity>(it->second);
            if (entity)
            {
                return entity;
            }
            else
            {
                LogError("Failed to cast entity to the requested type.");
            }
        }
        else
        {
            LogError("Entity with ID {} not found.", id.ToString());
        }
        return nullptr;
    }
    /**
     * @brief 更新资源
     * @tparam TEntity 实体类型，需继承自 IEntity
     * @param id 资源的唯一标识
     * @param entity 资源实体
     */
    template <typename TEntity>
        requires std::derived_from<TEntity, IEntity>
    void UpdateAsset(const UUID &id, std::shared_ptr<TEntity> entity)
    {
        using RepositoryType = typename RepositoryTraits<TEntity>::RepositoryType;
        static RepositoryType repository;
        auto it = mEntities.find(id);
        if (it != mEntities.end())
        {
            // 更新entity
            it->second = entity;
            repository.Update(entity);
            Serialize<TEntity>(entity->GetPath(), entity);
        }
    }
    /**
     * @brief 删除资源
     * @param id 资源的唯一标识
     */
    void DeleteAsset(const UUID &id)
    {
        auto it = mEntities.find(id);
        if (it != mEntities.end())
        {
            std::filesystem::remove(it->second->GetPath());
            mEntities.erase(it);
        }
    }
    /**
     * @brief 生成唯一的文件路径
     * @tparam TEntity 实体类型
     * @param baseName 基础名称
     * @return 唯一路径
     */
    template <typename TEntity>
    std::filesystem::path GenerateUniquePath(const std::filesystem::path &path, const std::string &baseName)
    {
        std::string ext = GetExtension(typeid(TEntity));
        std::filesystem::path sourcePath = path / (baseName + ext);
        int i = 0;
        while (std::filesystem::exists(sourcePath))
        {
            sourcePath = path / (baseName + std::to_string(i) + ext);
            i++;
        }
        return sourcePath;
    }
    std::string GetExtension(const std::type_index &type)
    {
        auto it = mTypeExtensions.find(type);
        return it != mTypeExtensions.end() ? it->second : ".unknown";
    }
    std::type_index GetTypeIndex(const std::string &extension)
    {
        auto it = mExtensionTypes.find(extension);
        if (it != mExtensionTypes.end())
        {
            return it->second;
        }
        return std::type_index(typeid(void));
    }
};
} // namespace MEngine