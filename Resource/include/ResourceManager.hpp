#pragma once

#include <concepts>
#include <filesystem>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "Entity/Entity.hpp"
#include "Entity/Material.hpp"
#include "Entity/Mesh.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Entity/Pipeline.hpp"
#include "Entity/Texture2D.hpp"
#include "Logger.hpp"
#include "Repository/MaterialRepository.hpp"
#include "Repository/MeshRepository.hpp"
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
    std::shared_ptr<Repository<Material>> mMaterialRepository;
    std::shared_ptr<Repository<Mesh>> mMeshRepository;
    std::shared_ptr<Repository<Pipeline>> mPipelineRepository;
    std::shared_ptr<Repository<Texture2D>> mTextureRepository;

    const std::unordered_map<std::type_index, std::string> mTypeToAssetExtensions = {
        {typeid(Pipeline), ".shader"},
        {typeid(PBRMaterial), ".pbrmaterial"},
        {typeid(Texture2D), ".tex2d"},
        {typeid(Mesh), ".mesh"},
        // {typeid(Material), ".material"}, {typeid(Animation), ".animation"}, {typeid(Model), ".model"},
        // {typeid(Audio), ".audio"},       {typeid(Scene), ".scene"},
    };
    const std::unordered_map<std::string, std::type_index> mAssetExtensionToTypes = {
        {".shader", typeid(Pipeline)},
        {".pbrmaterial", typeid(PBRMaterial)},
        {".tex2d", typeid(Texture2D)},
        {".mesh", typeid(Mesh)}
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
    ResourceManager()
    {
        mMaterialRepository = std::make_shared<MaterialRepository>();
        mMeshRepository = std::make_shared<MeshRepository>();
        mPipelineRepository = std::make_shared<PipelineRepository>();
        mTextureRepository = std::make_shared<Texture2DRepository>();
    }
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
    void CreateDefault()
    {
        mMaterialRepository->CreateDefault();
        mMeshRepository->CreateDefault();
        mPipelineRepository->CreateDefault();
        mTextureRepository->CreateDefault();
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    std::shared_ptr<TEntity> LoadAsset(const std::filesystem::path &path)
    {
        if constexpr (std::is_same_v<TEntity, Pipeline>)
        {
            return mPipelineRepository->LoadAsset(path);
        }
        else if constexpr (std::is_same_v<TEntity, Material>)
        {
            return mMaterialRepository->LoadAsset(path);
        }
        else if constexpr (std::is_same_v<TEntity, Texture2D>)
        {
            return mTextureRepository->LoadAsset(path);
        }
        else if constexpr (std::is_same_v<TEntity, Mesh>)
        {
            return mMeshRepository->LoadAsset(path);
        }
        else
        {
            LogError("Unsupported asset type: {}", typeid(TEntity).name());
            return nullptr;
        }
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
            else if (typeIdx == typeid(Mesh))
            {
                return LoadAsset<Mesh>(path);
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
    void SaveAsset(const UUID &id, const std::filesystem::path &path, std::string name = "New Asset")
    {
        auto sourcePath = GenerateUniquePath<TEntity>(path, name);
        if constexpr (std::is_same_v<TEntity, Pipeline>)
        {
            mPipelineRepository->SaveAsset(id, sourcePath);
        }
        else if constexpr (std::is_same_v<TEntity, Material>)
        {
            mMaterialRepository->SaveAsset(id, sourcePath);
        }
        else if constexpr (std::is_same_v<TEntity, Texture2D>)
        {
            mTextureRepository->SaveAsset(id, sourcePath);
        }
        else if constexpr (std::is_same_v<TEntity, Mesh>)
        {
            mMeshRepository->SaveAsset(id, sourcePath);
        }
        else
        {
            LogError("Unsupported asset type: {}", typeid(TEntity).name());
        }
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    std::shared_ptr<TEntity> CreateAsset()
    {
        if constexpr (std::is_same_v<TEntity, Pipeline>)
        {
            return mPipelineRepository->Create();
        }
        else if constexpr (std::is_same_v<TEntity, Material>)
        {
            return mMaterialRepository->Create();
        }
        else if constexpr (std::is_same_v<TEntity, Texture2D>)
        {
            return mTextureRepository->Create();
        }
        else if constexpr (std::is_same_v<TEntity, Mesh>)
        {
            return mMeshRepository->Create();
        }
        else
        {
            LogError("Unsupported asset type: {}", typeid(TEntity).name());
            return nullptr;
        }
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    std::shared_ptr<TEntity> GetAsset(const UUID &id)
    {
        if constexpr (std::is_same_v<TEntity, Pipeline>)
        {
            return mPipelineRepository->GetAsset(id);
        }
        else if constexpr (std::is_same_v<TEntity, Material>)
        {
            return mMaterialRepository->GetAsset(id);
        }
        else if constexpr (std::is_same_v<TEntity, Texture2D>)
        {
            return mTextureRepository->GetAsset(id);
        }
        else if constexpr (std::is_same_v<TEntity, Mesh>)
        {
            return mMeshRepository->GetAsset(id);
        }
        else
        {
            LogError("Unsupported asset type: {}", typeid(TEntity).name());
            return nullptr;
        }
    }
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    void UpdateAsset(std::shared_ptr<TEntity> entity)
    {
        if constexpr (std::is_same_v<TEntity, Pipeline>)
        {
            mPipelineRepository->UpdateAsset(entity);
        }
        else if constexpr (std::is_same_v<TEntity, Material>)
        {
            mMaterialRepository->UpdateAsset(entity);
        }
        else if constexpr (std::is_same_v<TEntity, Texture2D>)
        {
            mTextureRepository->UpdateAsset(entity);
        }
        else if constexpr (std::is_same_v<TEntity, Mesh>)
        {
            mMeshRepository->UpdateAsset(entity);
        }
        else
        {
            LogError("Unsupported asset type: {}", typeid(TEntity).name());
        }
    }
    /**
     * @brief 删除资源
     * @param id 资源的唯一标识
     */
    template <typename TEntity>
        requires std::derived_from<TEntity, Entity>
    void DeleteAsset(const UUID &id)
    {
        if constexpr (std::is_same_v<TEntity, Pipeline>)
        {
            mPipelineRepository->DeleteAsset(id);
        }
        else if constexpr (std::is_same_v<TEntity, Material>)
        {
            mMaterialRepository->DeleteAsset(id);
        }
        else if constexpr (std::is_same_v<TEntity, Texture2D>)
        {
            mTextureRepository->DeleteAsset(id);
        }
        else if constexpr (std::is_same_v<TEntity, Mesh>)
        {
            mMeshRepository->DeleteAsset(id);
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