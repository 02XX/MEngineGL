#pragma once
#include "Entity/IEntity.hpp"
#include "Entity/IMaterial.hpp"
#include "Math.hpp"
#include <vector>

namespace MEngine
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};
class IMesh : virtual public IEntity
{
  public:
    virtual ~IMesh() = default;
    virtual void SetVertexBuffer(const std::vector<Vertex> &vertices) = 0;
    virtual void SetIndexBuffer(const std::vector<uint32_t> &indices) = 0;
    virtual const std::vector<Vertex> &GetVertices() const = 0;
    virtual const std::vector<uint32_t> &GetIndices() const = 0;
    virtual GLuint GetVAO() const = 0;
};

} // namespace MEngine