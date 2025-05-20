#pragma once
#include "Entity/Entity.hpp"
#include "Math.hpp"
#include <vector>
namespace MEngine
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};
class Mesh final : public Entity
{
  public:
    std::vector<Vertex> Vertices;
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