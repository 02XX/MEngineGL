#include "Entity/Mesh.hpp"

namespace MEngine
{
Mesh::Mesh()
{
    Name = "Mesh";
    Type = EntityType::Mesh;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);
}
Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
void Mesh::Update()
{
    if (Vertices.empty() || Indices.empty())
    {
        return;
    }
    glNamedBufferStorage(VBO, static_cast<GLsizeiptr>(Vertices.size() * sizeof(Vertex)), Vertices.data(), 0);
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribBinding(VAO, 0, 0);

    glEnableVertexArrayAttrib(VAO, 1);
    glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(VAO, 1, 0);

    glEnableVertexArrayAttrib(VAO, 2);
    glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
    glVertexArrayAttribBinding(VAO, 2, 0);

    glNamedBufferStorage(EBO, static_cast<GLsizeiptr>(Indices.size() * sizeof(uint32_t)), Indices.data(), 0);
    glVertexArrayElementBuffer(VAO, EBO);
}
} // namespace MEngine