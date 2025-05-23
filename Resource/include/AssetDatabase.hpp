#pragma once
#include "Asset/Asset.hpp"
#include "Importer/AssetImporter.hpp"
#include "Importer/TextureImporter.hpp"
#include "UUID.hpp"
#include <memory>
#include <unordered_map>

namespace MEngine
{
namespace Editor
{
enum AssetType
{
    Folder,
    Material,
    Texture,
    Model,
    Prefab,
};
struct AssetMeta
{
    UUID ID;
    AssetType type;
    std::shared_ptr<AssetImporter> importer;
};
class AssetDatabase
{
  private:
    static std::unordered_map<UUID, AssetMeta> UUID2Meta;
    static std::unordered_map<std::filesystem::path, UUID> Path2UUID;

  public:
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
    static void CreateAsset(std::shared_ptr<const Asset> asset, const std::filesystem::path &path);
    /**
     * @brief 加载资源，如果资源已经加载过，则直接返回。注意：请仅在需要时加载资源，例如在编辑器中预览资源或在层级中时。
     *
     * @tparam T
     * @param path
     * @return std::shared_ptr<T>
     */
    template <typename T> static std::shared_ptr<T> LoadAssetAtPath(const std::filesystem::path &path)
    {
        if (auto it = Path2UUID.find(path); it != Path2UUID.end())
        {
            auto meta = UUID2Meta[it->second];
            // 根据meta的导入设置创建资源
            switch (meta.type)
            {
            case Folder:
            case Material:
            case Texture:
            case Model:
            case Prefab:
                break;
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
        j["type"] = magic_enum::enum_name(meta.type);
        auto importer = meta.importer;
        if (importer)
        {
            if (auto textureImporter = std::dynamic_pointer_cast<MEngine::Editor::TextureImporter>(importer))
            {
                j["assetPath"] = textureImporter->assetPath.string();
                j["name"] = textureImporter->name;
                j["TextureImporter"] = *textureImporter;
            }
            else
            {
                j["assetPath"] = importer->assetPath.string();
                j["name"] = importer->name;
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
        meta.ID = j.at("ID").get<MEngine::UUID>();
        meta.type = magic_enum::enum_cast<MEngine::Editor::AssetType>(j.at("type")).value();
        auto importer = j.at("DefaultImporter");
        if (importer)
        {
            if (importer.contains("TextureImporter"))
            {
                auto textureImporter = std::make_shared<MEngine::Editor::TextureImporter>();
                textureImporter->assetPath = importer.at("assetPath").get<std::string>();
                textureImporter->name = importer.at("name").get<std::string>();
                meta.importer = textureImporter;
            }
            else
            {
                auto defaultImporter = std::make_shared<MEngine::Editor::AssetImporter>();
                defaultImporter->assetPath = importer.at("assetPath").get<std::string>();
                defaultImporter->name = importer.at("name").get<std::string>();
                meta.importer = defaultImporter;
            }
        }
    }
};
} // namespace nlohmann
