#include "Entity/Mesh.hpp"
#include "Entity/IMaterial.hpp"

namespace MEngine
{
Mesh::Mesh()
{
    glCreateVertexArrays(1, &mVAO);
    glCreateBuffers(1, &mVBO);
    glCreateBuffers(1, &mEBO);
}
Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}
void Mesh::SetVertexBuffer(const std::vector<Vertex> &vertices)
{
    mVertices = vertices;
    glNamedBufferStorage(mVBO, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), 0);
    glVertexArrayVertexBuffer(mVAO, 0, mVBO, 0, sizeof(Vertex));

    glEnableVertexArrayAttrib(mVAO, 0);
    glVertexArrayAttribFormat(mVAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribBinding(mVAO, 0, 0);

    glEnableVertexArrayAttrib(mVAO, 1);
    glVertexArrayAttribFormat(mVAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(mVAO, 1, 0);

    glEnableVertexArrayAttrib(mVAO, 2);
    glVertexArrayAttribFormat(mVAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoords));
    glVertexArrayAttribBinding(mVAO, 2, 0);
}
void Mesh::SetIndexBuffer(const std::vector<uint32_t> &indices)
{
    mIndices = indices;
    mIndexCount = static_cast<uint32_t>(indices.size());

    glNamedBufferStorage(mEBO, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)), indices.data(), 0);
    glVertexArrayElementBuffer(mVAO, mEBO);
};
} // namespace MEngine