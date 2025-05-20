#pragma once
#include "Entity/Entity.hpp"
#include "Math.hpp"

#include <vector>
namespace MEngine
{
struct Vertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;
    Vec3 tangent;
    Vec3 bitangent;
};
class Mesh final : public Entity
{
  public:
    Property<std::vector<Vertex>> Vertices;
    std::vector<uint32_t> Indices;
    uint32_t IndexCount = 0;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

  public:
    Mesh();
    ~Mesh();
    void Update() override;
};
} // namespace MEngine