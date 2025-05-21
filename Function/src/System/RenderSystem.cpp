#include "System/RenderSystem.hpp"
#include "Component/TransformComponent.hpp"
#include "Entity/Entity.hpp"
#include "Logger.hpp"

namespace MEngine
{
RenderSystem::RenderSystem(std::shared_ptr<entt::registry> registry, std::shared_ptr<ResourceManager> resourceManager)
    : System(registry, resourceManager)
{
}
RenderSystem::~RenderSystem()
{
    glDeleteTextures(1, &ColorAttachment);
    glDeleteTextures(1, &DepthAttachment);
    glDeleteFramebuffers(1, &FBO);
}
void RenderSystem::Init()
{
    CreateFrameBuffer();
}
void RenderSystem::Update(float deltaTime)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearNamedFramebufferfv(FBO, GL_COLOR, 0, (GLfloat[]){0.2f, 0.3f, 0.3f, 1.0f});
    glClearNamedFramebufferfv(FBO, GL_DEPTH, 0, (GLfloat[]){1.0f});
    glEnable(GL_DEPTH_TEST);
    GetMainCamera();
    RenderQueue();
    UpdateSource();
    RenderForwardPass();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void RenderSystem::Shutdown()
{
}
void RenderSystem::CreateFrameBuffer(int width, int height)
{
    if (FBO != 0)
    {
        glDeleteTextures(1, &ColorAttachment);
        glDeleteTextures(1, &DepthAttachment);
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }
    glCreateFramebuffers(1, &FBO);
    glCreateTextures(GL_TEXTURE_2D, 1, &ColorAttachment);
    glTextureStorage2D(ColorAttachment, 1, GL_RGBA8, width, height);
    glCreateTextures(GL_TEXTURE_2D, 1, &DepthAttachment);
    glTextureStorage2D(DepthAttachment, 1, GL_DEPTH24_STENCIL8, width, height);

    glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, ColorAttachment, 0);
    glNamedFramebufferTexture(FBO, GL_DEPTH_STENCIL_ATTACHMENT, DepthAttachment, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogError("Framebuffer is not complete!");
    }
    glViewport(0, 0, width, height);
    LogInfo("Create framebuffer: {}x{}", width, height);
}
void RenderSystem::GetMainCamera()
{
    auto view = mRegistry->view<CameraComponent, TransformComponent>();
    for (auto entity : view)
    {
        auto &cameraComponent = view.get<CameraComponent>(entity);
        if (cameraComponent.isEditorCamera)
        {
            mMainCamera = cameraComponent;
            return;
        }
    }
    for (auto entity : view)
    {
        auto &cameraComponent = view.get<CameraComponent>(entity);
        if (cameraComponent.isMainCamera)
        {
            mMainCamera = cameraComponent;
            return;
        }
    }
}
void RenderSystem::UpdateSource()
{
    auto materialView = mRegistry->view<MaterialComponent>();
    for (auto entity : materialView)
    {
        auto &materialComponent = materialView.get<MaterialComponent>(entity);

        auto materialID = materialComponent.materialID;
        auto material = mResourceManager->GetAsset<Material>(materialID);
        if (material->MaterialType == MaterialType::PBR)
        {
            // auto pbrMaterial = std::dynamic_pointer_cast<PBRMaterial>(material);
            // auto albedoTexture = mResourceManager->GetAsset<Texture2D>(pbrMaterial->AlbedoTextureID);
            // auto normalTexture = mResourceManager->GetAsset<Texture2D>(pbrMaterial->NormalTextureID);
            // auto armTexture = mResourceManager->GetAsset<Texture2D>(pbrMaterial->ARMTextureID);
            // glBindTextureUnit(0, albedoTexture->mTextureID);
            // glBindSampler(0, albedoTexture->mSamplerID);
            // glBindTextureUnit(1, normalTexture->mTextureID);
            // glBindSampler(1, normalTexture->mSamplerID);
            // glBindTextureUnit(2, armTexture->mTextureID);
            // glBindSampler(2, armTexture->mSamplerID);
        }
        else if (material->MaterialType == MaterialType::Phong)
        {
            // auto phongMaterial = std::dynamic_pointer_cast<PhongMaterial>(material);
            // auto diffuseTexture = mResourceManager->GetAsset<Texture2D>(phongMaterial->DiffuseTextureID);
            // glBindTextureUnit(0, diffuseTexture->mTextureID);
            // glBindSampler(0, diffuseTexture->mSamplerID);
        }
        else if (material->MaterialType == MaterialType::Default)
        {
        }
        else
        {
            LogError("Unknown material type: {}", magic_enum::enum_name(material->MaterialType));
        }
        materialComponent.dirty = false;
    }
}

void RenderSystem::RenderQueue()
{
    mRenderQueue.clear();
    auto entities = mRegistry->view<TransformComponent, MeshComponent, MaterialComponent>();
    for (auto entity : entities)
    {
        auto &materialComponent = mRegistry->get<MaterialComponent>(entity);
        auto materialID = materialComponent.materialID;
        auto material = mResourceManager->GetAsset<Material>(materialID);
        mRenderQueue[material->PipelineID].push_back(entity);
    }
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
        glProgramUniformMatrix4fv(program, 0, 1, GL_FALSE,
                                  glm::value_ptr(mMainCamera.viewMatrix)); // layout(location = 1)
        glProgramUniformMatrix4fv(program, 1, 1, GL_FALSE,
                                  glm::value_ptr(mMainCamera.projectionMatrix)); // layout(location = 2)
        for (auto entity : entities)
        {
            auto &transformComponent = mRegistry->get<TransformComponent>(entity);
            auto &meshComponent = mRegistry->get<MeshComponent>(entity);
            auto &materialComponent = mRegistry->get<MaterialComponent>(entity);
            auto materialID = materialComponent.materialID;
            auto meshID = meshComponent.meshID;
            auto mesh = mResourceManager->GetAsset<Mesh>(meshID);
            glm::mat4 modelMatrix = transformComponent.modelMatrix;
            glProgramUniformMatrix4fv(program, 2, 1, GL_FALSE, glm::value_ptr(modelMatrix)); // layout(location =
            glBindVertexArray(mesh->VAO);
            glDrawElements(GL_TRIANGLES, mesh->Indices.size(), GL_UNSIGNED_INT, nullptr);
        }
    }
}
void RenderSystem::RenderPostProcessPass()
{
}
} // namespace MEngine