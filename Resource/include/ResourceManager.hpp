#pragma once

#include <concepts>
#include <filesystem>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "Entity/Entity.hpp"
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
    std::unordered_map<UUID, std::shared_ptr<Entity>> mEntities;
    const std::unordered_map<std::type_index, std::string> mTypeToAssetExtensions = {
        {typeid(Pipeline), ".shader"}, {typeid(PBRMaterial), ".pbrmaterial"}, {typeid(Texture2D), ".tex2d"}
        // {typeid(Material), ".material"}, {typeid(Animation), ".animation"}, {typeid(Model), ".model"},
        // {typeid(Audio), ".audio"},       {typeid(Scene), ".scene"},
    };
    const std::unordered_map<std::string, std::type_index> mAssetExtensionToTypes = {
        {".shader", typeid(Pipeline)}, {".pbrmaterial", typeid(PBRMaterial)}, {".tex2d", typeid(Texture2D)}
        // {".texture", typeid(Texture)},     {".mesh", typeid(Mesh)},
        // {".material", typeid(Material)},   {".animation", typeid(Animation)},
        // {".model", typeid(Model)},         {".audio", typeid(Audio)},
        // {".scene", typeid(Scene)},
    };
    const std::unordered_map<std::string, std::type_index> mRawExtensionToTypes = {
        {".png", typeid(Texture2D)}, {".jpg", typeid(Texture2D)}, {".jpeg", typeid(Texture2D)},
        {".bmp", typeid(Texture2D)}, {".tga", typeid(Texture2D)}, {".hdr", typeid(Texture2D)},
        {".exr", typeid(Texture2D)},
    };

  public:
    bool IsSupportRaw(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("File does not exist: " + path.string());
        }
        std::string extension = path.extension().string();
        auto it = mRawExtensionToTypes.find(extension);
        return it != mRawExtensionToTypes.end();
    }
    bool IsAsset(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("File does not exist: " + path.string());
        }
        std::string extension = path.extension().string();
        auto it = mAssetExtensionToTypes.find(extension);
        return it != mAssetExtensionToTypes.end();
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    std::shared_ptr<TEntity> LoadAsset(const std::filesystem::path &path)
    {
        auto entity = std::make_shared<TEntity>();
        Deserialize<TEntity>(path, entity);
        // 检查 ID 是否已存在
        if (mEntities.contains(entity->ID))
        {
            LogWarn("Entity with ID {} already exists.", entity->ID.Get().ToString());
        }
        // 更新相应的存储库
        using repoType = typename RepositoryTraits<TEntity>::RepositoryType;
        static repoType repository;
        repository.Update(entity);
        // 存储实体并返回 ID
        mEntities[entity->ID] = entity;
        LogInfo("Loaded asset: {}", path.string());
        return entity;
    }
    std::shared_ptr<Entity> LoadAsset(const std::filesystem::path &path)
    {
        // 检查文件是否存在
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("File does not exist: " + path.string());
        }
        std::string extension = path.extension().string();
        // 检查扩展名是否支持
        auto typeIndex = mAssetExtensionToTypes.find(extension);
        if (typeIndex == mAssetExtensionToTypes.end())
        {
            LogTrace("Unsupported file type: {}", extension);
            return nullptr;
        }
        // 根据类型索引分发加载逻辑
        const auto &typeIdx = typeIndex->second;
        if (typeIndex != mAssetExtensionToTypes.end())
        {
            if (typeIdx == typeid(Pipeline))
            {
                return LoadAsset<Pipeline>(path);
            }
            else if (typeIdx == typeid(PBRMaterial))
            {
                return LoadAsset<PBRMaterial>(path);
            }
            else if (typeIdx == typeid(Texture2D))
            {
                return LoadAsset<Texture2D>(path);
            }
            else
            {
                LogWarn("Not implement file type {}", mTypeToAssetExtensions.at(typeIndex->second));
                return nullptr;
            }
        }
        else
        {
            LogTrace("Unsupported file type: {}", extension);
            return nullptr;
        }
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    std::shared_ptr<TEntity> CreateAsset(const std::filesystem::path &path, std::string name = "New Asset")
    {
        if (!std::filesystem::exists(path))
        {
            LogError("Project path does not exist: {}", path.string());
            throw std::runtime_error("path does not exist: " + path.string());
        }
        using RepositoryType = typename RepositoryTraits<TEntity>::RepositoryType;
        static RepositoryType repository;
        auto entity = repository.Create();
        repository.Update(entity);
        mEntities[entity->ID] = entity;
        std::string ext = GetAssetExtensionFromType(typeid(TEntity));
        std::filesystem::path sourcePath = path / (name + ext);
        entity->SourcePath = sourcePath;
        Serialize(sourcePath, entity);
        return entity;
    }
    std::shared_ptr<Entity> CreateAsset(const std::filesystem::path &supportRawPath)
    {
        auto extension = supportRawPath.extension().string();
        if (extension.empty())
        {
            LogError("{} is must a file.", supportRawPath.string());
            return nullptr;
        }
        auto it = mRawExtensionToTypes.find(extension);
        if (it != mRawExtensionToTypes.end())
        {
            auto type = it->second;
            if (type == typeid(Pipeline))
            {
                auto pipeline = CreateAsset<Pipeline>(supportRawPath.parent_path(), supportRawPath.stem().string());
                return pipeline;
            }
            else if (type == typeid(PBRMaterial))
            {
                auto pbrMaterial =
                    CreateAsset<PBRMaterial>(supportRawPath.parent_path(), supportRawPath.stem().string());
                return pbrMaterial;
            }
            else if (type == typeid(Texture2D))
            {
                auto texture2D = CreateAsset<Texture2D>(supportRawPath.parent_path(), supportRawPath.stem().string());
                texture2D->ImagePath = supportRawPath;
                UpdateAsset(texture2D->ID, texture2D);
                return texture2D;
            }
            else
            {
                LogWarn("Unimplemented raw type: {}", extension);
                return nullptr;
            }
        }
        else
        {
            LogWarn("Unsupported raw type: {}", extension);
            return nullptr;
        }
    }

    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
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
        requires std::derived_from<TEntity, Entity>
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
        requires std::derived_from<TEntity, Entity>
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
     * @tparam TEntity 实体类型，需继承自 Entity
     * @param id 资源的唯一标识
     * @param entity 资源实体
     */
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
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
            Serialize<TEntity>(entity->SourcePath.Get(), entity);
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
            std::filesystem::remove(it->second->SourcePath.Get());
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
        std::string ext = GetAssetExtensionFromType(typeid(TEntity));
        std::filesystem::path sourcePath = path / (baseName + ext);
        int i = 0;
        while (std::filesystem::exists(sourcePath))
        {
            sourcePath = path / (baseName + std::to_string(i) + ext);
            i++;
        }
        return sourcePath;
    }
    std::string GetAssetExtensionFromType(const std::type_index &type)
    {
        auto it = mTypeToAssetExtensions.find(type);
        return it != mTypeToAssetExtensions.end() ? it->second : ".unknown";
    }
    std::type_index GetAssetTypeFromExtension(const std::string &extension)
    {
        auto it = mAssetExtensionToTypes.find(extension);
        if (it != mAssetExtensionToTypes.end())
        {
            return it->second;
        }
        return std::type_index(typeid(void));
    }
    std::type_index GetRawTypeFromExtension(const std::string &extension)
    {
        auto it = mRawExtensionToTypes.find(extension);
        if (it != mRawExtensionToTypes.end())
        {
            return it->second;
        }
        return std::type_index(typeid(void));
    }
};
} // namespace MEngine