
#include "AssetDatabase.hpp"
#include "Asset/Asset.hpp"
#include "Asset/CustomMaterial.hpp"
#include "Asset/PBRMaterial.hpp"
#include "Asset/PhongMaterial.hpp"
#include "Asset/Pipeline.hpp"
#include "Importer/NativeFormatImporter.hpp"
#include "Logger.hpp"
#include <fstream>
#include <memory>

namespace MEngine
{
namespace Editor
{
std::unordered_map<UUID, AssetMeta> AssetDatabase::UUID2Meta{};
std::unordered_map<std::filesystem::path, UUID> AssetDatabase::Path2UUID{};
std::vector<std::filesystem::path> AssetDatabase::AssetPaths{};
std::unordered_map<std::type_index, std::string> AssetDatabase::Asset2Extension{
    {typeid(Asset), ".asset"},     {typeid(Pipeline), ".shader"},   {typeid(Material), ".mat"},
    {typeid(PBRMaterial), ".mat"}, {typeid(PhongMaterial), ".mat"}, {typeid(CustomMaterial), ".mat"},
    {typeid(Prefab), ".prefab"},
};
void AssetDatabase::RegisterAssetDirectory(const std::filesystem::path &dir)
{
    if (!std::filesystem::exists(dir))
    {
        LogError("Asset directory does not exist: {}", dir.string());
        return;
    }
    AssetPaths.push_back(dir);
}
void AssetDatabase::UnregisterAssetDirectory(const std::filesystem::path &dir)
{
    auto it = std::remove(AssetPaths.begin(), AssetPaths.end(), dir);
    if (it != AssetPaths.end())
    {
        AssetPaths.erase(it, AssetPaths.end());
    }
}
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
        meta.importer->assetPath = path;
        meta.importer->name = path.stem().string();
        UUID2Meta[meta.ID] = meta;
        Path2UUID[path] = meta.ID;
        return;
    }
    // 构建meta
    AssetMeta meta;
    meta.ID = UUIDGenerator()();
    meta.IsFolder = false;
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
    {
        // 构建默认importer
        meta.importer = std::make_shared<TextureImporter>();
    }
    // for native asset
    if (extension == ".mat" || extension == ".shader" || extension == ".prefab")
    {
        meta.importer = std::make_shared<NativeFormatImporter>();
    }
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
std::filesystem::path AssetDatabase::GenerateUniqueAssetPath(std::filesystem::path path)
{
    if (!std::filesystem::exists(path))
    {
        return path;
    }
    const auto stem = path.stem();
    const auto extension = path.extension();
    auto parentPath = path.parent_path();

    int counter = 1;
    std::filesystem::path newPath;
    do
    {
        newPath = parentPath / (stem.string() + " (" + std::to_string(counter) + ")" + extension.string());
        ++counter;
    } while (std::filesystem::exists(newPath));
    return newPath;
}
void AssetDatabase::Refresh()
{
    for (auto &path : AssetPaths)
    {
        for (auto &entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (entry.is_regular_file())
            {
                auto path = entry.path();
                auto extension = path.extension();
                if (extension == ".meta")
                {
                    continue;
                }
                if (Path2UUID.find(path) == Path2UUID.end())
                {
                    ImportAsset(path);
                }
            }
        }
    }
}
} // namespace Editor
} // namespace MEngine