#pragma once
#include "Component/AssestComponent.hpp"
#include "Component/Component.hpp"
#include "Component/TextureComponent.hpp"
#include "Component/TransformComponent.hpp"
#include "Entity/Entity.hpp"
#include "Entity/PBRMaterial.hpp"
#include "Entity/Texture2D.hpp"
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>

using namespace entt::literals;
namespace MEngine
{
static void RegisterMeta()
{
    entt::meta<Entity>().type("Entity"_hs).data<&Entity::ID>("ID"_hs).data<&Entity::SourcePath>("SourcePath"_hs);
    entt::meta<Texture2D>().type("Texture2D"_hs).base<Entity>();
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