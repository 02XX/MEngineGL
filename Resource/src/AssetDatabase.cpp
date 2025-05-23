
#include "AssetDatabase.hpp"
#include "Asset/Asset.hpp"

namespace MEngine
{
void AssetDatabase::Scan(std::filesystem::path assetDir)
{
    auto entries = std::filesystem::directory_iterator(assetDir);
    for (auto &entry : entries)
    {
        if (entry.is_directory())
        {
            Scan(entry.path());
        }
        auto extension = entry.path().extension();
        if (extension == ".meta")
        {
            auto meta = LoadMeta(entry.path());
            LoadAsset(meta);
        }
        else
        {
            // 根据资源的后缀名判断资源类型
            // 根据类型构建对应默认的Importer
            // 根据Importer的SaveAndReimport方法保存资源，生成meta文件
            if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
            {
                // 纹理
                // TextureImporter textureImporter;
            }
            else if (extension == ".fbx" || extension == ".obj")
            {
                // 模型
                // ModelImporter modelImporter;
            }
            else if (extension == ".prefab")
            {
                // 预制体
            }
            else if (extension == ".mat")
            {
                // 材质
            }
            else
            {
                // 其他类型
            }
        }
    }
}
void AssetDatabase::Update()
{
    // 遍历UUID2Asset，更新标记为dirty的资源，并更新meta文件和资源文件
    for (auto &[id, asset] : UUID2Asset)
    {
        if (asset->isDirty)
        {
            // 1.根据asset的类型，进行分发
            // 2.如果未初始化图形资源，进行初始化
            // 3.保存资源和meta文件
            // 4.重新导入资源importer
            // 5.清除dirty标记
        }
    }
}
AssetMeta &AssetDatabase::AssetDatabase::LoadMeta(std::filesystem::path metaPath)
{
    if (!std::filesystem::exists(metaPath))
    {
        throw AssetMetaNotFoundException("Asset meta file not found: " + metaPath.string());
    }
    // 反序列化
    json j;
    std::ifstream file(metaPath);
    if (file.is_open())
    {
        j = json::parse(file);
        file.close();
        auto meta = j.get<AssetMeta>();
        UUID2Meta[meta.ID] = meta;
        Path2UUID[meta.assetPath] = meta.ID;
    }
    else
    {
        throw AssetMetaNotFoundException("Failed to open asset meta file: " + metaPath.string());
    }
}
std::shared_ptr<Asset> AssetDatabase::LoadAsset(const AssetMeta &meta)
{
    auto assetPath = meta.assetPath;
    if (!std::filesystem::exists(assetPath))
    {
        throw AssetNotFoundException("Asset file not found: " + assetPath.string());
    }
}
std::shared_ptr<Asset> AssetDatabase::LoadAsset(const std::filesystem::path &assetPath)
{
}
std::shared_ptr<Asset> AssetDatabase::LoadAsset(const UUID &id)
{
}
std::shared_ptr<Asset> AssetDatabase::GetAsset(const std::filesystem::path &assetPath)
{
}
std::shared_ptr<Asset> AssetDatabase::GetAsset(const UUID &id)
{
}
void AssetDatabase::UpdateAsset(const std::filesystem::path &assetPath)
{
}
void AssetDatabase::UpdateAsset(const UUID &id)
{
}
void AssetDatabase::SaveAsset(const UUID &id, const std::filesystem::path &savePath)
{
}
} // namespace MEngine