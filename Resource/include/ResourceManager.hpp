#pragma once

#include <filesystem>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "Entity/Entity.hpp"
#include "Entity/IEntity.hpp"
#include "Entity/Material.hpp"
#include "Entity/Shader.hpp"
#include "Entity/Texture2D.hpp"
#include "Logger.hpp"
#include "Repository/MaterialRepository.hpp"
#include "Repository/Repository.hpp"
#include "Repository/ShaderRepository.hpp"
#include "Repository/TextureRepository.hpp"
#include "UUID.hpp"

using json = nlohmann::json;
namespace MEngine
{
class ResourceManager
{
  private:
    std::filesystem::path mResourcePath = std::filesystem::current_path();

  private:
    std::unordered_map<UUID, std::shared_ptr<IEntity>> mEntities;
    const std::unordered_map<std::type_index, std::string> mTypeExtensions = {
        {typeid(Shader), ".shader"},
        {typeid(PBRMaterial), ".pbrmaterial"},
        {typeid(Texture2D), ".tex2d"},
        {typeid(Texture2D), ".texture"},
        // {typeid(Material), ".material"}, {typeid(Animation), ".animation"}, {typeid(Model), ".model"},
        // {typeid(Audio), ".audio"},       {typeid(Scene), ".scene"},
    };
    const std::unordered_map<std::string, std::type_index> mExtensionTypes = {
        {".shader", typeid(Shader)}, {".pbrmaterial", typeid(PBRMaterial)}, {".tex2d", typeid(Texture2D)}
        // {".texture", typeid(Texture)},     {".mesh", typeid(Mesh)},
        // {".material", typeid(Material)},   {".animation", typeid(Animation)},
        // {".model", typeid(Model)},         {".audio", typeid(Audio)},
        // {".scene", typeid(Scene)},
    };

  public:
    /**
     * @brief 加载资源
     *
     */
    UUID LoadAsset(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("File does not exist: " + path.string());
        }
        // 获取文件类型
        std::string extension = path.extension().string();
        auto typeIndex = mExtensionTypes.find(extension);
        std::shared_ptr<IEntity> entity = nullptr;
        if (typeIndex != mExtensionTypes.end())
        {
            if (typeIndex->second == typeid(Shader))
            {
                entity = std::make_shared<Shader>();
                Deserialize<Shader>(path, std::dynamic_pointer_cast<Shader>(entity));
                static ShaderRepository repository;
                repository.Update(std::dynamic_pointer_cast<Shader>(entity));
            }
            else if (typeIndex->second == typeid(PBRMaterial))
            {
                entity = std::make_shared<PBRMaterial>();
                Deserialize<PBRMaterial>(path, std::dynamic_pointer_cast<PBRMaterial>(entity));
                static PBRMaterialRepository pbrRepository;
                pbrRepository.Update(std::dynamic_pointer_cast<PBRMaterial>(entity));
            }
            else if (typeIndex->second == typeid(Texture2D))
            {
                entity = std::make_shared<Texture2D>();
                Deserialize<Texture2D>(path, std::dynamic_pointer_cast<Texture2D>(entity));
                static Texture2DRepository textureRepository;
                textureRepository.Update(std::dynamic_pointer_cast<Texture2D>(entity));
            }
        }
        if (entity)
        {
            mEntities[entity->GetID()] = entity;
            LogInfo("Loaded asset: {}", path.string());
            return entity->GetID();
        }
        else
        {
            LogError("Failed to load asset: {}", path.string());
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
    UUID CreateAsset()
    {
        using RepositoryType = typename RepositoryTraits<TEntity>::RepositoryType;
        static RepositoryType repository;
        auto entity = repository.Create();
        repository.Update(entity);
        mEntities[entity->GetID()] = entity;
        std::filesystem::path sourcePath = GenerateUniquePath<TEntity>(entity->GetName());
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
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + path.string());
        }
        json j = json::parse(file);
        *target = j.get<TEntity>();
    }
    /**
     * @brief 获取资源
     * @param id 资源的唯一标识
     * @return std::shared_ptr<TEntity> 资源实体
     */
    template <typename TEntity>
        requires std::derived_from<TEntity, IEntity>
    std::shared_ptr<IEntity> GetAsset(const UUID &id)
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
            Serialize<TEntity>(entity.GetPath(), entity);
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
    template <typename TEntity> std::filesystem::path GenerateUniquePath(const std::string &baseName)
    {
        std::string ext = GetExtension(typeid(TEntity));
        std::filesystem::path path = mResourcePath / ("New " + baseName + ext);
        int i = 0;
        while (std::filesystem::exists(path))
        {
            path = mResourcePath / ("New " + baseName + std::to_string(i) + ext);
            i++;
        }
        return path;
    }
    std::string GetExtension(const std::type_index &type)
    {
        auto it = mTypeExtensions.find(type);
        return it != mTypeExtensions.end() ? it->second : ".unknown";
    }
};
} // namespace MEngine