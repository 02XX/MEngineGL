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
    // glm::vec3 tangent;
    // glm::vec3 bitangent;
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

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Vertex>
{
    static void to_json(json &j, const MEngine::Vertex &v)
    {
        j = json{{"position", {v.position.x, v.position.y, v.position.z}},
                 {"normal", {v.normal.x, v.normal.y, v.normal.z}},
                 {"texCoord", {v.texCoord.x, v.texCoord.y}}};
    }
    static void from_json(const json &j, MEngine::Vertex &v)
    {
        auto pos = j.at("position");
        v.position = glm::vec3(pos[0], pos[1], pos[2]);
        auto norm = j.at("normal");
        v.normal = glm::vec3(norm[0], norm[1], norm[2]);
        auto tex = j.at("texCoord");
        v.texCoord = glm::vec2(tex[0], tex[1]);
    }
};
template <> struct adl_serializer<MEngine::Mesh>
{
    static void to_json(json &j, const MEngine::Mesh &mesh)
    {
        j = static_cast<MEngine::Entity>(mesh);
        j["Vertices"] = mesh.Vertices;
        j["Indices"] = mesh.Indices;
    }
    static void from_json(const json &j, MEngine::Mesh &mesh)
    {
        static_cast<MEngine::Entity &>(mesh) = j;
        mesh.Vertices = j.at("Vertices").get<std::vector<MEngine::Vertex>>();
        mesh.Indices = j.at("Indices").get<std::vector<uint32_t>>();
        mesh.IndexCount = static_cast<uint32_t>(mesh.Indices.size());
    }
};
} // namespace nlohmann