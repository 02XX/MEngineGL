#include "System/RenderSystem.hpp"
#include "Component/TransformComponent.hpp"
#include "Entity/Entity.hpp"
#include "Logger.hpp"
#include "Component/LightComponent.hpp"

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
    glDeleteBuffers(1,&LightUBO);
}
void RenderSystem::Init()
{
    glCreateBuffers(1, &LightUBO);
    glNamedBufferStorage(LightUBO, sizeof(Light) * 8, nullptr, GL_DYNAMIC_STORAGE_BIT);
    CreateFrameBuffer();
    GLint maxUBOBindings;
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUBOBindings);
    LogInfo("Max UBO Bindings Supported: {}", maxUBOBindings);
}
void RenderSystem::Update(float deltaTime)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    GLfloat clearColor[] = { 0.2f, 0.3f, 0.3f, 1.0f };
    GLfloat clearDepth[] = { 1.0f };
    glClearNamedFramebufferfv(FBO, GL_COLOR, 0, clearColor);
    glClearNamedFramebufferfv(FBO, GL_DEPTH, 0, clearDepth);
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
        switch (materialComponent.type)
        {
        case MaterialType::Standard:
            break;
        case MaterialType::PBR:
            break;
        case MaterialType::Phong:
            break;
        default:
            LogError("Unknown material type");

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
        std::shared_ptr<Material> material;
        switch (materialComponent.type)
        {
        case MaterialType::Standard:
            material = mResourceManager->GetAsset<StandardMaterial>(materialID);
            break;
        case MaterialType::PBR:
            material = mResourceManager->GetAsset<PBRMaterial>(materialID);
            break;
        case MaterialType::Phong:
            break;
        }
        mRenderQueue[material->PipelineID].push_back(entity);
    }
}
void RenderSystem::RenderDeferredPass()
{
}
void RenderSystem::RenderForwardPass()
{
    auto lightView = mRegistry->view<LightComponent,TransformComponent>();
    int count = 0;
    std::vector<Light> lights;
    for (auto entity : lightView)
    {
        Light light;
        if (count > 8) return;
        auto &lightComponent = lightView.get<LightComponent>(entity);
        auto &lightTransformComponent = lightView.get<TransformComponent>(entity);
        light.color = lightComponent.Color;
        light.position = lightTransformComponent.worldPosition;
        switch (lightComponent.LightType)
        {
        case LightType::Directional:
            {
                light.type = 0;
                light.direction = lightTransformComponent.worldRotation * glm::vec3(0,0,-1);
                break;
            }
        case LightType::Point:
            {
                light.type = 1;
                light.radius = 10;
                break;
            }
        case LightType::Spot:
            {
                break;
            }
        }
        count++;
        lights.push_back(light);
    }
    for (auto [pipelineID, entities] : mRenderQueue)
    {
        auto pipeline = mResourceManager->GetAsset<Pipeline>(pipelineID);
        auto program = pipeline->programID;
        glUseProgram(program);
        glProgramUniformMatrix4fv(program, 0, 1, GL_FALSE,
                                  glm::value_ptr(mMainCamera.viewMatrix)); // layout(location = 1)
        glProgramUniformMatrix4fv(program, 1, 1, GL_FALSE,
                                  glm::value_ptr(mMainCamera.projectionMatrix)); // layout(location = 2)

        // glProgramUniform3f(program,3,1,GL_FALSE,glm::value_ptr(lights[0].color));

        for (auto entity : entities)
        {
            auto &transformComponent = mRegistry->get<TransformComponent>(entity);
            auto &meshComponent = mRegistry->get<MeshComponent>(entity);
            auto &materialComponent = mRegistry->get<MaterialComponent>(entity);
            auto materialID = materialComponent.materialID;
            auto modelID = meshComponent.modelID;
            auto meshIndex = meshComponent.meshIndex;
            auto model = mResourceManager->GetAsset<Model>(modelID);
            auto mesh = model->Meshes[meshIndex];
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