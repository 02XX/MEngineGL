#include "System/RenderSystem.hpp"

namespace MEngine
{
RenderSystem::RenderSystem(std::shared_ptr<entt::registry> registry) : System(registry)
{
}
void RenderSystem::Init()
{
}
void RenderSystem::Update(float deltaTime)
{
}
void RenderSystem::Shutdown()
{
}
void RenderSystem::GetMainCamera()
{
    auto view = mRegistry->view<CameraComponent>();
    for (auto entity : view)
    {
        auto &cameraComponent = view.get<CameraComponent>(entity);
        if (cameraComponent.isMainCamera)
        {
            mMainCamera = cameraComponent;
            break;
        }
    }
}
void RenderSystem::RenderQueue()
{
    auto entities = mRegistry->view<TransformComponent, MeshComponent, MaterialComponent>();
    for (auto entity : entities)
    {
        auto &materialComponent = mRegistry->get<MaterialComponent>(entity);
        // auto material = materialComponent.material;
        // mRenderQueue[material->GetPipelineID()].push_back(entity);
    }
}
void RenderSystem::RenderShadowPass()
{
}
void RenderSystem::RenderDeferredPass()
{
}
void RenderSystem::RenderForwardPass()
{
    for (auto [pipelineID, entities] : mRenderQueue)
    {
        auto pipeline = mResourceManager->GetAsset<Pipeline>(pipelineID);
        auto program = pipeline->programID;
        glUseProgram(program);
        for (auto entity : entities)
        {

            // auto &transformComponent = mRegistry->get<TransformComponent>(entity);
            // auto &meshComponent = mRegistry->get<MeshComponent>(entity);
            // auto &materialComponent = mRegistry->get<MaterialComponent>(entity);
            // auto material = materialComponent.material;
            // switch (material->GetMaterialType())
            // {
            // case MaterialType::PBR: {
            //     auto pbrMaterial = std::dynamic_pointer_cast<PBRMaterial>(material);
            //     // 传入变换矩阵
            //     glm::mat4 modelMatrix = transformComponent.worldMatrix;
            //     glProgramUniformMatrix4fv(program, 0, 1, GL_FALSE, glm::value_ptr(modelMatrix)); // layout(location =
            //     0) glProgramUniformMatrix4fv(program, 1, 1, GL_FALSE,
            //                               glm::value_ptr(mMainCamera.viewMatrix)); // layout(location = 1)
            //     glProgramUniformMatrix4fv(program, 2, 1, GL_FALSE,
            //                               glm::value_ptr(mMainCamera.projectionMatrix)); // layout(location = 2)
            //     // 更新材质参数
            //     auto pbrProperties = pbrMaterial->GetPBRProperties();
            //     glProgramUniform1f(program, 0, pbrProperties.roughness); // layout(location = 0)
            //     glProgramUniform1f(program, 1, pbrProperties.metallic);  // layout(location = 1)
            //     glProgramUniform3f(program, 2, pbrProperties.color.r, pbrProperties.color.g,
            //                        pbrProperties.color.b); // layout(location = 2)
            //     // 更新纹理
            //     auto albedoTexture = mResourceManager->GetAsset<Texture2D>(pbrMaterial->GetAlbedoTexture());
            //     auto normalTexture = mResourceManager->GetAsset<Texture2D>(pbrMaterial->GetNormalTexture());
            //     auto armTexture = mResourceManager->GetAsset<Texture2D>(pbrMaterial->GetARMTexture());
            //     glBindTextureUnit(0, albedoTexture->GetTextureID());
            //     glBindSampler(0, albedoTexture->GetSamplerID());
            //     glBindTextureUnit(1, normalTexture->GetTextureID());
            //     glBindSampler(1, normalTexture->GetSamplerID());
            //     glBindTextureUnit(2, armTexture->GetTextureID());
            //     glBindSampler(2, armTexture->GetSamplerID());
            //     glBindVertexArray(meshComponent.mesh->GetVAO());
            //     glDrawElements(GL_TRIANGLES, meshComponent.mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
            //     break;
            // }
            // case MaterialType::Phong:
            //     break;
            // }
        }
    }
}
void RenderSystem::RenderPostProcessPass()
{
}
} // namespace MEngine