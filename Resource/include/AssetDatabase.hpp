#pragma once
#include "Asset/Asset.hpp"
#include "UUID.hpp"
#include <memory>
#include <unordered_map>

namespace MEngine
{
class AssetMetaNotFoundException : public std::exception
{
  private:
    std::string message;

  public:
    AssetMetaNotFoundException(const std::string &message) : message(message)
    {
    }
    const char *what() const noexcept override
    {
        return message.c_str();
    }
};
class AssetNotFoundException : public std::exception
{
  private:
    std::string message;

  public:
    AssetNotFoundException(const std::string &message) : message(message)
    {
    }
    const char *what() const noexcept override
    {
        return message.c_str();
    }
};
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
    std::filesystem::path assetPath;
    AssetType type;
    AssetImporter importer;
};
class AssetDatabase
{
  private:
    std::unordered_map<UUID, AssetMeta> UUID2Meta;
    std::unordered_map<std::filesystem::path, UUID> Path2UUID;
    std::unordered_map<UUID, std::shared_ptr<Asset>> UUID2Asset;
    
  public:
    /**
     * @brief 扫描资源目录，构建资源元数据数据库，对于没有meta的资源，生成meta文件
     *
     * @param assetDir 资源目录
     */
    void Scan(std::filesystem::path assetDir);
    /**
     * @brief 扫描已经加载的资源，更新标记为dirty的资源，并更新meta文件和资源文件
     *
     * @param assetDir 资源目录
     * @param metaPath meta文件路径
     */
    void Update();
    /**
     * @brief 加载meta资源，构建资源元数据数据库
     *
     * @param metaPath meta文件路径
     * @throw AssetMetaNotFoundException 如果资源未找到
     * @return AssetMeta& 返回加载的资源元数据
     */
    AssetMeta &LoadMeta(std::filesystem::path metaPath);
    /**
     * @brief 根据meta加载资源
     *
     * @param meta 资源元数据
     * @throw AssetNotFoundException 如果资源未找到
     * @return std::shared_ptr<Asset>
     */
    std::shared_ptr<Asset> LoadAsset(const AssetMeta &meta);
    /**
     * @brief 根据资源路径加载资源，查询Path2UUID, UUID2Meta后根据meta加载资源
     *
     * @param assetPath 资源路径
     * @throw AssetMetaNotFoundException 如果资源未找到
     * @return std::shared_ptr<Asset>
     */
    std::shared_ptr<Asset> LoadAsset(const std::filesystem::path &assetPath);
    /**
     * @brief 根据UUID加载资源，查询UUID2Meta后根据meta加载资源
     *
     * @param id 资源UUID
     * @throw AssetMetaNotFoundException 如果资源未找到
     * @return std::shared_ptr<Asset>
     */
    std::shared_ptr<Asset> LoadAsset(const UUID &id);
    /**
     * @brief 根据资源路径获取资源，查询Path2UUID, UUID2Asset后返回资源
     *
     * @param assetPath 资源路径
     * @throw AssetNotFoundException 如果资源未找到
     * @return std::shared_ptr<Asset>
     */
    std::shared_ptr<Asset> GetAsset(const std::filesystem::path &assetPath);
    /**
     * @brief 根据UUID获取资源，查询UUID2Asset后返回资源
     *
     * @param id 资源UUID
     * @throw AssetNotFoundException 如果资源未找到
     * @return std::shared_ptr<Asset>
     */
    std::shared_ptr<Asset> GetAsset(const UUID &id);
    /**
     * @brief 根据资源路径更新资源，查询Path2UUID, UUID2Asset后更新资源
     *
     * @param assetPath 资源路径
     * @throw AssetNotFoundException 如果资源未找到
     */
    void UpdateAsset(const std::filesystem::path &assetPath);
    /**
     * @brief 根据UUID更新资源，查询UUID2Asset后更新资源
     *
     * @param id 资源UUID
     * @throw AssetNotFoundException 如果资源未找到
     */
    void UpdateAsset(const UUID &id);
    /**
     * @brief 保存资源，查询Path2UUID, UUID2Asset后保存资源，并生成meta文件和资源文件
     *
     * @param savePath 保存路径
     * @throw AssetNotFoundException 如果资源未找到
     */
    void SaveAsset(const UUID &id, const std::filesystem::path &savePath);
    /**
     * @brief 删除资源，查询Path2UUID, UUID2Asset后删除资源，并删除meta文件和资源文件
     *
     * @param assetPath 资源路径
     * @throw AssetNotFoundException 如果资源未找到
     */
    void DeleteAsset(const UUID &id);
    /**
     * @brief 删除资源，查询Path2UUID, UUID2Asset后删除资源，并删除meta文件和资源文件
     *
     * @param assetPath 资源路径
     * @throw AssetNotFoundException 如果资源未找到
     */
    void DeleteAsset(const std::filesystem::path &assetPath);

    template <class TAsset> std::shared_ptr<TAsset> LoadAsset(const std::filesystem::path &assetPath)
    {
        auto asset = LoadAsset(assetPath);
        return std::dynamic_pointer_cast<TAsset>(asset);
    }
    template <class TAsset> std::shared_ptr<TAsset> LoadAsset(const UUID &id)
    {
        auto asset = LoadAsset(id);
        return std::dynamic_pointer_cast<TAsset>(asset);
    }
    template <class TAsset> std::shared_ptr<TAsset> GetAsset(const std::filesystem::path &assetPath)
    {
        auto asset = GetAsset(assetPath);
        return std::dynamic_pointer_cast<TAsset>(asset);
    }
    template <class TAsset> std::shared_ptr<TAsset> GetAsset(const UUID &id)
    {
        auto asset = GetAsset(id);
        return std::dynamic_pointer_cast<TAsset>(asset);
    }
};

} // namespace MEngine
