#pragma once
#include "Asset/Asset.hpp"
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
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
class Mesh final : public Asset
{
  public:
    std::vector<Vertex> Vertices{};
    std::vector<uint32_t> Indices{};

  private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

  public:
    Mesh();
    ~Mesh();
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
        j = static_cast<MEngine::Asset>(mesh);
        j["Vertices"] = mesh.Vertices;
        j["Indices"] = mesh.Indices;
    }
    static void from_json(const json &j, MEngine::Mesh &mesh)
    {
        static_cast<MEngine::Asset &>(mesh) = j;
        mesh.Vertices = j.at("Vertices").get<std::vector<MEngine::Vertex>>();
        mesh.Indices = j.at("Indices").get<std::vector<uint32_t>>();
    }
};
} // namespace nlohmann