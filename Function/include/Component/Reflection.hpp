#pragma once
#include "Component/AssestComponent.hpp"
#include "Component/Component.hpp"
#include "Component/TextureComponent.hpp"
#include "Component/TransformComponent.hpp"
#include "Entity/Entity.hpp"
#include "Entity/IEntity.hpp"
#include "Entity/ITexture.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Entity/Texture2D.hpp"
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>

using namespace entt::literals;
namespace MEngine
{
static void RegisterMeta()
{
    entt::meta<IEntity>()
        .type("IEntity"_hs)
        .data<&IEntity::SetID, &IEntity::GetID>("id"_hs)
        .data<&IEntity::SetPath, &IEntity::GetPath>("path"_hs)
        .data<&IEntity::SetName, &IEntity::GetName>("name"_hs);
    entt::meta<Entity>().base<IEntity>().type("Entity"_hs);
    entt::meta<ITexture>()
        .type("ITexture"_hs)
        .base<IEntity>()
        .data<&ITexture::SetImagePath, &ITexture::GetImagePath>("imagePath"_hs)
        .data<&ITexture::SetImporter, &ITexture::GetImporter>("importer"_hs);
    entt::meta<Texture2D>().type("Texture2D"_hs).base<ITexture>();
    entt::meta<Component>().type("Component"_hs).data<&Component::dirty>("dirty"_hs);
    entt::meta<AssetsComponent>()
        .type("AssetsComponent"_hs)
        .base<Component>()
        .data<&AssetsComponent::path>("path"_hs)
        .data<&AssetsComponent::name>("name"_hs)
        .data<&AssetsComponent::type>("type"_hs);
    entt::meta<TextureComponent>()
        .type("TextureComponent"_hs)
        .base<Component>()
        .data<&TextureComponent::textureID>("texture"_hs);
    entt::meta<TransformComponent>()
        .type("TransformComponent"_hs)
        .base<Component>()
        .data<&TransformComponent::name>("name"_hs)
        .data<&TransformComponent::localPosition>("localPosition"_hs)
        .data<&TransformComponent::localRotation>("localRotation"_hs)
        .data<&TransformComponent::localScale>("localScale"_hs)
        .data<&TransformComponent::worldPosition>("worldPosition"_hs)
        .data<&TransformComponent::worldRotation>("worldRotation"_hs)
        .data<&TransformComponent::worldScale>("worldScale"_hs);
}
} // namespace MEngine