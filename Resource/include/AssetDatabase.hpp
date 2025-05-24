#pragma once
#include "Asset/Asset.hpp"
#include "Asset/CustomMaterial.hpp"
#include "Asset/Folder.hpp"
#include "Asset/Material.hpp"
#include "Asset/PBRMaterial.hpp"
#include "Asset/PhongMaterial.hpp"
#include "Asset/Pipeline.hpp"
#include "Asset/Prefab.hpp"
#include "Asset/Texture.hpp"
#include "Asset/Texture2D.hpp"
#include "Importer/AssetImporter.hpp"
#include "Importer/AudioImporter.hpp"
#include "Importer/NativeFormatImporter.hpp"
#include "Importer/PrefabImporter.hpp"
#include "Importer/ShaderImporter.hpp"
#include "Importer/TextureImporter.hpp"
#include "Logger.hpp"
#include "UUID.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace MEngine::Core;
namespace MEngine
{

namespace Editor
{
struct AssetMeta
{
    UUID ID;
    bool IsFolder = false;
    AssetType Type = AssetType::None;
    std::shared_ptr<AssetImporter> importer;
};
class AssetDatabase
{
  private:
    static std::unordered_map<UUID, std::shared_ptr<AssetMeta>> UUID2Meta;
    static std::unordered_map<std::filesystem::path, UUID> Path2UUID;
    static std::unordered_map<std::type_index, std::string> Asset2Extension;
    static std::vector<std::filesystem::path> AssetPaths;

  public:
    static AssetType DetermineAssetType(const std::string &extension);
    static void RegisterAssetDirectory(const std::filesystem::path &dir);
    static void UnregisterAssetDirectory(const std::filesystem::path &dir);
    static void CreateFolder(const std::filesystem::path &parentFolder, const std::string &newFolderName);
    static void MoveAsset(const std::filesystem::path &srcPath, const std::filesystem::path &dstPath);
    static void RenameAsset(const std::filesystem::path &path, const std::string &newName);
    static void DeleteAsset(const std::filesystem::path &path);
    static void CopyAsset(const std::filesystem::path &srcPath, const std::filesystem::path &dstPath);
    /**
     * @brief 导入资源，并且创建资源的元数据，构建数据库
     *
     * @param path
     */
    static void ImportAsset(const std::filesystem::path &path);
    /**
     * @brief 创建原生的资源，例如.mat, .shader等
     *
     * @param asset
     * @param path
     */
    template <std::derived_from<Asset> TAsset>
    static void CreateAsset(std::shared_ptr<const TAsset> asset, const std::filesystem::path &path)
    {

        if constexpr (std::is_same_v<TAsset, Folder>)
        {
            if (std::filesystem::create_directory(GenerateUniqueAssetPath(path)))
            {
                ImportAsset(path);
            }
            else
            {
                LogError("Failed to create folder: {}", path.string());
                return;
            }
        }
        else
        {
            auto extension = path.extension();
            auto assetPath = path;
            auto ti = std::type_index(typeid(*asset));
            if (auto it = Asset2Extension.find(ti); it != Asset2Extension.end())
            {
                if (extension != it->second)
                {
                    assetPath.replace_extension(it->second);
                    LogWarn("Asset extension mismatch: {} != {}, changed to {}", extension.string(), it->second,
                            assetPath.string());
                }
                auto extension = it->second;
                json j;
                j = *asset;
                std::ofstream assetFile(GenerateUniqueAssetPath(assetPath));
                assetFile << j.dump(4);
                assetFile.close();
                ImportAsset(assetPath);
            }
            else
            {
                LogError("Asset type not supported to create: {}", ti.name());
            }
        }
    }
    static void Refresh();
    static std::filesystem::path GenerateUniqueAssetPath(std::filesystem::path path);
    static std::shared_ptr<AssetMeta> GetAssetMeta(const std::filesystem::path &path);
    /**
     * @brief 加载资源，如果资源已经加载过，则直接返回。注意：请仅在需要时加载资源，例如在编辑器中预览资源或在层级中时。
     *
     * @tparam T
     * @param path
     * @return std::shared_ptr<T>
     */
    template <std::derived_from<Asset> TAsset>
    static std::shared_ptr<TAsset> LoadAssetAtPath(const std::filesystem::path &path)
    {
        if (auto it = Path2UUID.find(path); it != Path2UUID.end())
        {
            auto meta = UUID2Meta[it->second];
            // 根据meta的导入设置创建资源
            if constexpr (std::is_same_v<TAsset, Folder>)
            {
                auto folder = std::make_shared<Folder>();
                return folder;
            }
            else if constexpr (std::is_same_v<TAsset, Pipeline>)
            {
            }
            else if constexpr (std::is_same_v<TAsset, PBRMaterial>)
            {
                auto pbrMaterial = std::make_shared<PBRMaterial>();
                return pbrMaterial;
            }
            else if constexpr (std::is_same_v<TAsset, Texture>)
            {
                auto texture = std::make_shared<Texture>();
                return texture;
            }
            else if constexpr (std::is_same_v<TAsset, PhongMaterial>)
            {
                auto phongMaterial = std::make_shared<PhongMaterial>();
                return phongMaterial;
            }
            else if constexpr (std::is_same_v<TAsset, Prefab>)
            {
                auto prefab = std::make_shared<Prefab>();
                return prefab;
            }
            else
            {
                throw std::runtime_error("Unsupported asset type");
            }
        }
        else
        {
            throw std::runtime_error("Asset not imported: " + path.string());
        }
    }
};
} // namespace Editor
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Editor::AssetMeta>
{
    static void to_json(json &j, const MEngine::Editor::AssetMeta &meta)
    {
        j["ID"] = meta.ID;
        j["folder"] = meta.IsFolder;
        auto importer = meta.importer;
        if (importer)
        {
            if (auto textureImporter = std::dynamic_pointer_cast<MEngine::Editor::TextureImporter>(importer))
            {
                j["TextureImporter"] = *textureImporter;
            }
            else if (auto defaultImporter = std::dynamic_pointer_cast<MEngine::Editor::NativeFormatImporter>(importer))
            {
                j["NativeFormatImporter"] = *defaultImporter;
            }
            else if (auto assetImporter = std::dynamic_pointer_cast<MEngine::Editor::AudioImporter>(importer))
            {
                j["AudioImporter"] = *assetImporter;
            }
            else if (auto nativeImporter = std::dynamic_pointer_cast<MEngine::Editor::ShaderImporter>(importer))
            {
                j["ShaderImporter"] = *nativeImporter;
            }
            else if (auto shaderImporter = std::dynamic_pointer_cast<MEngine::Editor::PrefabImporter>(importer))
            {
                j["PrefabImporter"] = *shaderImporter;
            }
            else
            {
                j["DefaultImporter"] = *importer;
            }
        }
        else
        {
            throw std::runtime_error("Importer is null");
        }
    }
    static void from_json(const json &j, MEngine::Editor::AssetMeta &meta)
    {
        meta.ID = j.at("ID");
        meta.IsFolder = j.at("folder").get<bool>();
        if (j.contains("TextureImporter"))
        {
            auto textureImporter = j.at("TextureImporter").get<MEngine::Editor::TextureImporter>();
            meta.importer = std::make_shared<MEngine::Editor::TextureImporter>(textureImporter);
        }
        else if (j.contains("DefaultImporter"))
        {
            auto defaultImporter = j.at("DefaultImporter").get<MEngine::Editor::AssetImporter>();
            meta.importer = std::make_shared<MEngine::Editor::AssetImporter>(defaultImporter);
        }
        else if (j.contains("NativeFormatImporter"))
        {
            auto defaultImporter = j.at("NativeFormatImporter").get<MEngine::Editor::NativeFormatImporter>();
            meta.importer = std::make_shared<MEngine::Editor::NativeFormatImporter>(defaultImporter);
        }
        else if (j.contains("AudioImporter"))
        {
            auto audioImporter = j.at("AudioImporter").get<MEngine::Editor::AudioImporter>();
            meta.importer = std::make_shared<MEngine::Editor::AudioImporter>(audioImporter);
        }
        else if (j.contains("ShaderImporter"))
        {
            auto shaderImporter = j.at("ShaderImporter").get<MEngine::Editor::ShaderImporter>();
            meta.importer = std::make_shared<MEngine::Editor::ShaderImporter>(shaderImporter);
        }
        else if (j.contains("PrefabImporter"))
        {
            auto prefabImporter = j.at("PrefabImporter").get<MEngine::Editor::PrefabImporter>();
            meta.importer = std::make_shared<MEngine::Editor::PrefabImporter>(prefabImporter);
        }
        else
        {
            throw std::runtime_error("Invalid asset meta importer");
        }
    }
};
} // namespace nlohmann
