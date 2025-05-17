#pragma once
#include "Entity/Entity.hpp"
#include "Entity/IMaterial.hpp"
#include "Entity/IMesh.hpp"

#include <vector>
namespace MEngine
{

class Mesh final : public IMesh, public Entity
{
  private:
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;
    uint32_t mIndexCount = 0;
    GLuint mVAO = 0;
    GLuint mVBO = 0;
    GLuint mEBO = 0;

  public:
    Mesh();
    ~Mesh();
    void SetVertexBuffer(const std::vector<Vertex> &vertices) override;
    void SetIndexBuffer(const std::vector<uint32_t> &indices) override;
    inline const std::vector<Vertex> &GetVertices() const override
    {
        return mVertices;
    }
    inline const std::vector<uint32_t> &GetIndices() const override
    {
        return mIndices;
    }
    inline GLuint GetVAO() const override
    {
        return mVAO;
    }
    inline uint32_t GetIndexCount() const override
    {
        return mIndexCount;
    }
    void Update();
};
} // namespace MEngine