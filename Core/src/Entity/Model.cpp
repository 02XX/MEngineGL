#include "Entity/Model.hpp"
#include <memory>

namespace MEngine
{
Model::Model()
{
    Name = "Model";
    Type = EntityType::Model;
}
void Model::Update()
{
    if (!ModelPath.empty())
    {
        if (!std::filesystem::exists(ModelPath))
        {
            throw std::runtime_error("File does not exist: " + ModelPath.string());
        }
        std::string extension = ModelPath.extension().string();
        if (extension != ".fbx")
        {
            throw std::runtime_error("File is not a FBX file: " + ModelPath.string());
        }
        // 读取FBX文件
        Assimp::Importer importer;
        auto scene = importer.ReadFile(ModelPath.string(), aiProcess_GenUVCoords | aiProcess_ForceGenNormals |
                                                               aiProcess_Triangulate | aiProcess_CalcTangentSpace |
                                                               aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            throw std::runtime_error("Load fbx file failed.");
        }
        Meshes.reserve(scene->mNumMeshes);
        for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            auto mesh = scene->mMeshes[i];
            auto meshEntity = std::make_shared<Mesh>();
            // 处理网格数据
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                if (mesh->HasNormals())
                {
                    vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                }
                if (mesh->HasTextureCoords(0))
                {
                    vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                }
                meshEntity->Vertices.push_back(vertex);
            }
            for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                auto face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                {
                    meshEntity->Indices.push_back(face.mIndices[j]);
                }
            }
            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            }
            Meshes.push_back(meshEntity);
        }
        std::function<std::shared_ptr<Node>(const aiNode *, const aiScene *)> ProcessNode = [&](const aiNode *node,
                                                                                                const aiScene *scene) {
            auto currentNode = std::make_shared<Node>();
            currentNode->Name = node->mName.C_Str();
            currentNode->Meshes.reserve(node->mNumMeshes);
            auto transform = node->mTransformation;
            currentNode->Transform =
                glm::mat4(transform.a1, transform.b1, transform.c1, transform.d1, transform.a2, transform.b2,
                          transform.c2, transform.d2, transform.a3, transform.b3, transform.c3, transform.d3,
                          transform.a4, transform.b4, transform.c4, transform.d4);
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                currentNode->Meshes.push_back(node->mMeshes[i]);
            }
            // 递归处理子节点
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                auto childNode = ProcessNode(node->mChildren[i], scene);
                currentNode->Children.push_back(childNode);
            }
            return currentNode;
        };
        RootNode = ProcessNode(scene->mRootNode, scene);
    }
    if (!Meshes.empty())
    {
        for (auto mesh : Meshes)
        {
            mesh->Update();
        }
    }
}
} // namespace MEngine
