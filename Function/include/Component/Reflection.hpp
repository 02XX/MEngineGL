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
struct Info
{
    std::string DisplayName = "Unknown";
    bool Editable = false;
    bool Serializable = true;
};
static void RegisterMeta()
{
    entt::meta<Entity>()
        .type("Entity"_hs)
        .custom<Info>(Info{
            .DisplayName = "Entity",
        })
        .data<&Entity::ID>("ID"_hs)
        .custom<Info>(Info{.DisplayName = "ID", .Serializable = false})
        .data<&Entity::SourcePath>("SourcePath"_hs)
        .custom<Info>(Info{.DisplayName = "SourcePath", .Serializable = false});
    entt::meta<Texture2D>()
        .type("Texture2D"_hs)
        .base<Entity>()
        .custom<Info>(Info{
            .DisplayName = "Texture2D",
        })
        .data<&Texture2D::Width>("Width"_hs)
        .custom<Info>(Info{
            .DisplayName = "Width",
        })
        .data<&Texture2D::Height>("Height"_hs)
        .custom<Info>(Info{
            .DisplayName = "Height",
        })
        .data<&Texture2D::Channels>("Channels"_hs)
        .custom<Info>(Info{
            .DisplayName = "Channels",
        })
        .data<&Texture2D::mSamplerID>("mSamplerID"_hs)
        .custom<Info>(Info{
            .DisplayName = "mSamplerID",
        })
        .data<&Texture2D::test>("test"_hs)
        .custom<Info>(Info{.DisplayName = "test", .Editable = true})
        .data<&Texture2D::mTextureID>("mTextureID"_hs)
        .custom<Info>(Info{
            .DisplayName = "mTextureID",
        });
    // .data<&Texture2D::Importer>("Importer"_hs)
    // .custom<Info>(Info{
    //     .DisplayName = "Importer",
    // });
    entt::meta<Importer>()
        .type("Importer"_hs)
        .custom<Info>(Info{
            .DisplayName = "Importer",
        })
        .data<&Importer::mipLevels>("mip_levels"_hs)
        .custom<Info>(Info{
            .DisplayName = "mip_levels",
        })
        .data<&Importer::minFilter>("min_filter"_hs)
        .custom<Info>(Info{
            .DisplayName = "min_filter",
        })
        .data<&Importer::magFilter>("mag_filter"_hs)
        .custom<Info>(Info{
            .DisplayName = "mag_filter",
        })
        .data<&Importer::wrapS>("wrap_s"_hs)
        .custom<Info>(Info{
            .DisplayName = "wrap_s",
        })
        .data<&Importer::wrapT>("wrap_t"_hs)
        .custom<Info>(Info{
            .DisplayName = "wrap_t",
        })
        .data<&Importer::wrapR>("wrap_r"_hs)
        .custom<Info>(Info{
            .DisplayName = "wrap_r",
        });
    entt::meta<Component>()
        .type("Component"_hs)
        .custom<Info>(Info{
            .DisplayName = "Component",
        })
        .data<&Component::dirty>("dirty"_hs)
        .custom<Info>(Info{
            .DisplayName = "dirty",
            .Serializable = false,
        });
    entt::meta<AssetsComponent>()
        .type("AssetsComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "AssetsComponent",
        })
        .base<Component>()
        .data<&AssetsComponent::path>("path"_hs)
        .custom<Info>(Info{
            .DisplayName = "path",
        })
        .data<&AssetsComponent::name>("name"_hs)
        .custom<Info>(Info{
            .DisplayName = "name",
        })
        .data<&AssetsComponent::type>("type"_hs)
        .custom<Info>(Info{
            .DisplayName = "Asset Type",
        });
    entt::meta<TextureComponent>()
        .type("TextureComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "TextureComponent",
        })
        .base<Component>()
        .data<&TextureComponent::textureID>("texture"_hs)
        .custom<Info>(Info{.DisplayName = "Texture ID", .Editable = true});
    entt::meta<TransformComponent>()
        .type("TransformComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "TransformComponent",
        })
        .base<Component>()
        .data<&TransformComponent::name>("name"_hs)
        .custom<Info>(Info{
            .DisplayName = "name",
            .Editable = true,
        })
        .data<&TransformComponent::localPosition>("localPosition"_hs)
        .custom<Info>(Info{
            .DisplayName = "localPosition",
            .Editable = true,
        })
        .data<&TransformComponent::localRotation>("localRotation"_hs)
        .custom<Info>(Info{
            .DisplayName = "localRotation",
            .Editable = true,
        })
        .data<&TransformComponent::localScale>("localScale"_hs)
        .custom<Info>(Info{
            .DisplayName = "localScale",
            .Editable = true,
        })
        .data<&TransformComponent::worldPosition>("worldPosition"_hs)
        .custom<Info>(Info{
            .DisplayName = "worldPosition",
            .Editable = true,
        })
        .data<&TransformComponent::worldRotation>("worldRotation"_hs)
        .custom<Info>(Info{
            .DisplayName = "worldRotation",
            .Editable = true,
        })
        .data<&TransformComponent::worldScale>("worldScale"_hs)
        .custom<Info>(Info{
            .DisplayName = "worldScale",
            .Editable = true,
        });
}
} // namespace MEngine