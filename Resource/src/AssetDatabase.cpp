
#include "AssetDatabase.hpp"

namespace MEngine
{
namespace Editor
{
std::unordered_map<UUID, AssetMeta> AssetDatabase::UUID2Meta{};
std::unordered_map<std::filesystem::path, UUID> AssetDatabase::Path2UUID{};
void AssetDatabase::ImportAsset(const std::filesystem::path &path)
{
    auto extension = path.extension();
    if (extension == ".meta")
    {
        LogWarn("Please do not import .meta file");
        return;
    }
    auto metaPath = path;
    metaPath += ".meta";
    if (std::filesystem::exists(metaPath))
    {
        LogTrace("Dserialize from existing meta file");
        std::ifstream metaFile(metaPath);
        if (!metaFile.is_open())
        {
            LogError("Failed to open meta file: {}", metaPath.string());
            return;
        }
        json j;
        metaFile >> j;
        metaFile.close();
        auto meta = j.get<AssetMeta>();
        UUID2Meta[meta.ID] = meta;
        Path2UUID[path] = meta.ID;
        return;
    }
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
    {
        // 构建meta
        AssetMeta meta;
        meta.ID = UUIDGenerator()();
        meta.type = AssetType::Texture;
        // 构建默认importer
        meta.importer = std::make_shared<TextureImporter>();
        meta.importer->assetPath = path;
        meta.importer->name = path.stem().string();
        json j;
        j = meta;
        std::ofstream metaFile(metaPath);
        metaFile << j.dump(4);
        if (!metaFile.is_open())
        {
            LogError("Failed to open meta file: {}", metaPath.string());
            return;
        }
        metaFile.close();
    }
}
void AssetDatabase::CreateAsset(std::shared_ptr<const Asset> asset, const std::filesystem::path &path)
{
}
} // namespace Editor
} // namespace MEngine