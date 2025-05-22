#pragma once

#include <cmath>
#include <concepts>
#include <entt/entity/fwd.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "BasicGeometryFactory.hpp"
#include "Entity/Entity.hpp"
#include "Entity/Folder.hpp"
#include "Entity/Material.hpp"
#include "Entity/Mesh.hpp"
#include "Entity/Model.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Entity/Pipeline.hpp"
#include "Entity/Texture2D.hpp"
#include "Logger.hpp"
#include "Repository/FolderRepository.hpp"
#include "Repository/StandardMaterialRepository.hpp"
#include "Repository/MeshRepository.hpp"
#include "Repository/ModelRepository.hpp"
#include "Repository/PipelineRepository.hpp"
#include "Repository/Repository.hpp"
#include "Repository/TextureRepository.hpp"
#include "UUID.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>

using json = nlohmann::json;
namespace MEngine
{

class ResourceManager
{
    template <typename T> struct RepositoryTraits;

  private:
    BasicGeometryFactory mBasicGeometryFactory;
    std::filesystem::path mProjectPath{};

  private:
    std::shared_ptr<IRepository<StandardMaterial>> mStandardMaterialRepository;
    std::shared_ptr<IRepository<PBRMaterial>> mPBRMaterialRepository;
    std::shared_ptr<IRepository<Mesh>> mMeshRepository;
    std::shared_ptr<IRepository<Pipeline>> mPipelineRepository;
    std::shared_ptr<IRepository<Texture2D>> mTextureRepository;
    std::shared_ptr<IRepository<Model>> mModelRepository;
    std::shared_ptr<IRepository<Folder>> mFolderRepository;
    std::unordered_map<PrimitiveType, UUID> mGeometries;

    std::unordered_map<std::filesystem::path, std::shared_ptr<Entity>> mCachedAssets;

    const std::unordered_map<std::type_index, std::string> mTypeToAssetExtensions = {
        {typeid(Pipeline), ".shader"},
        {typeid(PBRMaterial), ".pbrmaterial"},
        {typeid(Texture2D), ".tex2d"},
        {typeid(Mesh), ".mesh"},
        {typeid(Model), ".model"},
        {typeid(Folder), ".folder"}
        // {typeid(Material), ".material"}, {typeid(Animation), ".animation"}, {typeid(Model), ".model"},
        // {typeid(Audio), ".audio"},       {typeid(Scene), ".scene"},
    };
    const std::unordered_map<std::string, std::type_index> mAssetExtensionToTypes = {
        {".shader", typeid(Pipeline)},
        {".pbrmaterial", typeid(PBRMaterial)},
        {".tex2d", typeid(Texture2D)},
        {".mesh", typeid(Mesh)},
        {".model", typeid(Model)},
        {".folder", typeid(Folder)}
        // {".texture", typeid(Texture)},     {".mesh", typeid(Mesh)},
        // {".material", typeid(Material)},   {".animation", typeid(Animation)},
        // {".model", typeid(Model)},         {".audio", typeid(Audio)},
        // {".scene", typeid(Scene)},
    };
    const std::unordered_map<std::string, std::type_index> mRawExtensionToType = {
        {".fbx", typeid(Model)}, {".png", typeid(Texture2D)}
        // {".obj", typeid(Mesh)}, {".fbx", typeid(Model)}, {".gltf", typeid(Model)},
        // {".glb", typeid(Model)}, {".stl", typeid(Mesh)}, {".3ds", typeid(Mesh)},
        // {".dae", typeid(Model)}, {".blend", typeid(Model)}, {".ply", typeid(Mesh)},
        // {".dxf", typeid(Mesh)},  {".3mf", typeid(Mesh)},  {".abc", typeid(Mesh)},
    };

  public:
    void SetProjectPath(const std::filesystem::path& projectPath)
    {
        mProjectPath = projectPath;
    }
    ResourceManager()
    {
        mStandardMaterialRepository = std::make_shared<StandardMaterialRepository>();
        mMeshRepository = std::make_shared<MeshRepository>();
        mPipelineRepository = std::make_shared<PipelineRepository>();
        mTextureRepository = std::make_shared<Texture2DRepository>();
        mModelRepository = std::make_shared<ModelRepository>();
        mFolderRepository = std::make_shared<FolderRepository>();
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
    void CreateDefault() const
    {
        mStandardMaterialRepository->CreateDefault();
        mMeshRepository->CreateDefault();
        mPipelineRepository->CreateDefault();
        mTextureRepository->CreateDefault();
        mModelRepository->CreateDefault();
        mPBRMaterialRepository->CreateDefault();
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    std::shared_ptr<TEntity> LoadAsset(const std::filesystem::path &path)
    {
        auto entity = GetRepository<TEntity>()->LoadAsset(path);
        mCachedAssets[path] = entity;
        return entity;
    }
    std::shared_ptr<Entity> LoadAsset(const std::filesystem::path &path)
    {
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
        auto typeIdx = typeIndex->second;
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
            else if (typeIdx == typeid(Mesh))
            {
                return LoadAsset<Mesh>(path);
            }
            else if (typeIdx == typeid(Model))
            {
                return LoadAsset<Model>(path);
            }
            else if (typeIdx == typeid(Folder))
            {
                return LoadAsset<Folder>(path);
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
    void LoadRawAsset(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("File does not exist: " + path.string());
        }
        std::shared_ptr<Entity> entity;
        std::filesystem::path assetPath = path;
        if (std::filesystem::is_directory(path))
        {
            assetPath += ".folder";
            if (std::filesystem::exists(assetPath))
            {
                LogTrace("File already exists: {}", assetPath.string());
                return;
            }
            auto folder = CreateAsset<Folder>();
            folder->SourcePath = assetPath;
            folder->FolderPath = path;
            folder->Type = EntityType::Folder;
            folder->Name = path.stem().string();
            UpdateAsset(folder);
            entity = folder;
        }
        else
        {
            std::string extension = path.extension().string();
            auto typeIndex = mRawExtensionToType.find(extension);
            if (typeIndex != mRawExtensionToType.end())
            {
                auto typeIdx = typeIndex->second;
                auto type = mTypeToAssetExtensions.find(typeIdx);
                assetPath.replace_extension(type->second);
                if (std::filesystem::exists(assetPath))
                {
                    LogTrace("File already exists: {}", assetPath.string());
                    return;
                }
                if (typeIdx == typeid(Model))
                {
                    auto model = CreateAsset<Model>();
                    model->SourcePath = assetPath;
                    model->Name = path.stem().string();
                    model->ModelPath = path;
                    model->Type = EntityType::Model;
                    UpdateAsset(model);
                    entity = model;
                }
                else if (typeIdx == typeid(Texture2D))
                {
                    auto texture = CreateAsset<Texture2D>();
                    texture->SourcePath = assetPath;
                    texture->Name = path.stem().string();
                    texture->ImagePath = path;
                    texture->Type = EntityType::Texture2D;
                    UpdateAsset(texture);
                    entity = texture;
                }
                else
                {
                    LogWarn("Not implement file type {}", extension);
                }
            }
            else
            {
                LogWarn("Not Support file type {}", extension);
            }
        }
        mCachedAssets[assetPath] = entity;
    }
    std::shared_ptr<Entity> GetAsset(const std::filesystem::path &path)
    {
        auto it = mCachedAssets.find(path);
        if (it != mCachedAssets.end())
        {
            return it->second;
        }
        else
        {
            LogWarn("Asset not found in cache: {}", path.string());
            return nullptr;
        }
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    std::shared_ptr<TEntity> CreateAsset()
    {
        auto entity = GetRepository<TEntity>()->Create();
        return std::dynamic_pointer_cast<TEntity>(entity);
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    std::shared_ptr<TEntity> GetAsset(const UUID &id)
    {
        auto entity = GetRepository<TEntity>()->GetAsset(id);
        return std::dynamic_pointer_cast<TEntity>(entity);
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    void UpdateAsset(std::shared_ptr<TEntity> entity)
    {
        auto repository = GetRepository<TEntity>();
        repository->UpdateAsset(entity);
    }
    /**
     * @brief 删除资源
     * @param id 资源的唯一标识
     */
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    void DeleteAsset(const UUID &id)
    {
        auto repository = GetRepository<TEntity>();
        repository->DeleteAsset(id);
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    void SaveAsset(const UUID &id, const std::filesystem::path &path, std::string name = "New Asset")
    {
        auto repository = GetRepository<TEntity>();
        auto entity = repository->GetAsset(id);
        if (entity)
        {
            entity->SourcePath = GenerateUniquePath<TEntity>(path, name);
            entity->Name = name;
            repository->SaveAsset(id, path);
        }
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    void SaveAsset(std::shared_ptr<TEntity> entity, const std::filesystem::path &path, std::string name = "New Asset")
    {
        SaveAsset<TEntity>(entity->ID, path, name);
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
    std::shared_ptr<Model> GetBasicGeometry(PrimitiveType type)
    {
        auto it = mGeometries.find(type);
        if (it != mGeometries.end())
        {
            return GetAsset<Model>(it->second);
        }
        else
        {
            auto [vertices, indices] = mBasicGeometryFactory.GetGeometry(type);
            auto model = CreateAsset<Model>();
            std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
            mesh->Vertices = vertices;
            mesh->Indices = indices;
            model->Meshes.push_back(mesh);
            model->Update();
            mGeometries[type] = model->ID;
            return model;
        }
    }
    std::vector<std::shared_ptr<Entity>> GetAssetsFromDirectory(const std::filesystem::path &path)
    {
        std::vector<std::shared_ptr<Entity>> assets;
        for (auto entity : mCachedAssets)
        {
            if (entity.first.parent_path() == path)
            {
                assets.push_back(entity.second);
            }
        }
        return assets;
    }
    template <typename TEntity> auto &GetRepository()
    {
        return RepositoryTraits<TEntity>::repository(*this);
    }
};

template <> struct ResourceManager::RepositoryTraits<Pipeline>
{
    static auto &repository(ResourceManager &rm)
    {
        return rm.mPipelineRepository;
    }
};

template <> struct ResourceManager::RepositoryTraits<PBRMaterial>
{
    static auto &repository(ResourceManager &rm)
    {
        return rm.mPBRMaterialRepository;
    }
};
template <> struct MEngine::ResourceManager::RepositoryTraits<StandardMaterial>
{
    static auto &repository(ResourceManager &rm)
    {
        return rm.mStandardMaterialRepository;
    }
};
template <> struct ResourceManager::RepositoryTraits<Texture2D>
{
    static auto &repository(ResourceManager &rm)
    {
        return rm.mTextureRepository;
    }
};

template <> struct ResourceManager::RepositoryTraits<Mesh>
{
    static auto &repository(ResourceManager &rm)
    {
        return rm.mMeshRepository;
    }
};

template <> struct ResourceManager::RepositoryTraits<Model>
{
    static auto &repository(ResourceManager &rm)
    {
        return rm.mModelRepository;
    }
};
template <> struct ResourceManager::RepositoryTraits<Folder>
{
    static auto &repository(ResourceManager &rm)
    {
        return rm.mFolderRepository;
    }
};

} // namespace MEngine