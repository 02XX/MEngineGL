#pragma once
#include "Entity/Entity.hpp"
#include <memory>
namespace MEngine
{
template <typename TEntity>
    requires std::derived_from<TEntity, Entity>
class IRepository
{
  public:
    virtual ~IRepository() = default;
    virtual void CreateDefault() = 0;
    /**
     * @brief 创建空的资源实体
     *
     * @return std::shared_ptr<TEntity>
     */
    virtual std::shared_ptr<TEntity> Create() = 0;
    virtual std::shared_ptr<TEntity> GetAsset(const UUID &id) = 0;
    /**
     * @brief 更新资源，如果资源有相应的资源文件，则会自动更新文件
     *
     * @param entity
     */
    virtual void UpdateAsset(std::shared_ptr<TEntity> entity) = 0;
    virtual void DeleteAsset(const UUID &id) = 0;
    /**
     * @brief 从文件中加载资源
     *
     * @param path 资源的路径，必须是支持的资源类型，比如.shader, .pbrmaterial, .tex2d, .mesh
     * @return std::shared_ptr<TEntity>
     */
    virtual std::shared_ptr<TEntity> LoadAsset(const std::filesystem::path &path) = 0;
    /**
     * @brief 保存资源到文件
     *
     * @param path 资源的路径，必须是支持的资源类型，比如.shader, .pbrmaterial, .tex2d, .mesh
     * @return std::shared_ptr<TEntity>
     */
    virtual void SaveAsset(const UUID &id, const std::filesystem::path &path) = 0;
};
} // namespace MEngine