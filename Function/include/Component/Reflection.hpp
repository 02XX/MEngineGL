#pragma once
#include "Asset/Asset.hpp"
#include "Asset/Folder.hpp"
#include "Asset/PBRMaterial.hpp"
#include "Asset/Texture2D.hpp"
#include "Component/AssestComponent.hpp"
#include "Component/CameraComponent.hpp"
#include "Component/Component.hpp"
#include "Component/MaterialComponent.hpp"
#include "Component/MeshComponent.hpp"
#include "Component/TextureComponent.hpp"
#include "Component/TransformComponent.hpp"
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>

#include "LightComponent.hpp"

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
    entt::meta<Asset>()
        .type("Asset"_hs)
        .custom<Info>(Info{
            .DisplayName = "Asset",
            .Serializable = false,
        })
        .data<&Asset::Name>("Name"_hs)
        .custom<Info>(Info{.DisplayName = "Name", .Editable = false});
    // entt::meta<Texture2D>()
    //     .type("Texture2D"_hs)
    //     .base<Asset>()
    //     .custom<Info>(Info{
    //         .DisplayName = "Texture2D",
    //     })
    //     .data<&Texture2D::Width>("Width"_hs)
    //     .custom<Info>(Info{
    //         .DisplayName = "Width",
    //     })
    //     .data<&Texture2D::Height>("Height"_hs)
    //     .custom<Info>(Info{
    //         .DisplayName = "Height",
    //     })
    //     .data<&Texture2D::Channels>("Channels"_hs)
    //     .custom<Info>(Info{
    //         .DisplayName = "Channels",
    //     })
    //     .data<&Texture2D::mSamplerID>("mSamplerID"_hs)
    //     .custom<Info>(Info{
    //         .DisplayName = "mSamplerID",
    //     })
    //     .data<&Texture2D::mTextureID>("mTextureID"_hs)
    //     .custom<Info>(Info{
    //         .DisplayName = "mTextureID",
    //     });
    // .data<&Texture2D::Importer>("Importer"_hs)
    // .custom<Info>(Info{
    //     .DisplayName = "Importer",
    // });
    entt::meta<Component>()
        .type("Component"_hs)
        .custom<Info>(Info{
            .DisplayName = "Component",
            .Serializable = false,
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
    entt::meta<CameraComponent>()
        .type("CameraComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "CameraComponent",
        })
        .base<Component>()
        .data<&CameraComponent::isMainCamera>("isMainCamera"_hs)
        .custom<Info>(Info{
            .DisplayName = "isMainCamera",
            .Editable = true,
        })
        .data<&CameraComponent::aspectRatio>("aspectRatio"_hs)
        .custom<Info>(Info{
            .DisplayName = "aspectRatio",
            .Editable = true,
        })
        .data<&CameraComponent::fovX>("fovX"_hs)
        .custom<Info>(Info{
            .DisplayName = "fov",
            .Editable = true,
        })
        .data<&CameraComponent::fovY>("fovY"_hs)
        .custom<Info>(Info{
            .DisplayName = "fovY",
            .Editable = true,
        })
        .data<&CameraComponent::zoom>("zoom"_hs)
        .custom<Info>(Info{
            .DisplayName = "zoom",
            .Editable = true,
        });
    entt::meta<MeshComponent>()
        .type("MeshComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "MeshComponent",
        })
        .base<Component>()
        .data<&MeshComponent::modelID>("modelID"_hs)
        .custom<Info>(Info{
            .DisplayName = "modelID",
            .Editable = true,
        })
        .data<&MeshComponent::meshIndex>("meshIndex"_hs)
        .custom<Info>(Info{
            .DisplayName = "meshIndex",
            .Editable = true,
        });
    // entt::meta<MaterialComponent>()
    //     .type("MaterialComponent"_hs)
    //     .custom<Info>(Info{
    //         .DisplayName = "MaterialComponent",
    //     })
    //     .base<Component>()
    //     .data<&MaterialComponent::materialID>("materialID"_hs)
    //     .custom<Info>(Info{
    //         .DisplayName = "materialID",
    //         .Editable = true,
    //     });
    entt::meta<PBRMaterial>()
        .type("PBRMaterial"_hs)
        .custom<Info>(Info{
            .DisplayName = "PBRMaterial",
        })
        .base<Material>()
        .data<&PBRMaterial::AlbedoTextureID>("albedo"_hs)
        .custom<Info>(Info{
            .DisplayName = "Albedo",
            .Editable = true,
        })
        .data<&PBRMaterial::ARMTextureID>("arm"_hs)
        .custom<Info>(Info{
            .DisplayName = "Roughness",
            .Editable = true,
        })
        .data<&PBRMaterial::NormalTextureID>("normal"_hs)
        .custom<Info>(Info{
            .DisplayName = "Metallic",
            .Editable = true,
        })
        .data<&PBRMaterial::Parameters>("parameters"_hs)
        .custom<Info>(Info{
            .DisplayName = "Parameters",
            .Editable = true,
        });
    entt::meta<PBRParameters>()
        .type("PBRParameters"_hs)
        .custom<Info>(Info{
            .DisplayName = "PBRParameters",
        })
        .data<&PBRParameters::metallic>("metallic"_hs)
        .custom<Info>(Info{
            .DisplayName = "Metallic",
            .Editable = true,
        })
        .data<&PBRParameters::roughness>("roughness"_hs)
        .custom<Info>(Info{
            .DisplayName = "Roughness",
            .Editable = true,
        });
    entt::meta<Folder>()
        .type("Folder"_hs)
        .custom<Info>(Info{
            .DisplayName = "Folder",
        })
        .base<Asset>()
        .data<&Folder::FolderPath>("FolderPath"_hs)
        .custom<Info>(Info{
            .DisplayName = "FolderPath",
            .Editable = true,
        });
    entt::meta<LightComponent>()
        .type("LightComponent"_hs)
        .custom<Info>(Info{.DisplayName = "LightComponent"})
        .base<Component>()
        .data<&LightComponent::LightType>("LightType"_hs)
        .custom<Info>(Info{.DisplayName = "LightType", .Editable = true})
        .data<&LightComponent::Color>("Color"_hs)
        .custom<Info>(Info{.DisplayName = "Color", .Editable = true})
        .data<&LightComponent::Intensity>("Intensity"_hs)
        .custom<Info>(Info{.DisplayName = "Intensity", .Editable = true})
        .data<&LightComponent::Radius>("Radius"_hs)
        .custom<Info>(Info{.DisplayName = "Radius", .Editable = true});
}
} // namespace MEngine