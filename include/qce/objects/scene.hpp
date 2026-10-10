// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>

#include <qce/objects/texture.hpp>
#include <qce/objects/figures.hpp>

#include <variant>

namespace QCE {
    using ResourceParams = std::variant<
        CuboidParams,
        SphereParams,
        PlaneParams,
        MeshParams,
        TextureParams,
        MaterialParams,
        AnimationParams
    >;

    struct ResourceDescription {
        std::string    name;
        ResourceParams params;
    };

    struct DynamicMeshDescription {
        std::string base;
        std::string deformated;
    };

    struct TransformAnimationDescription {
        std::string animation_name;
        bool is_looped = false;
    };

    struct EntityDescription {
        std::optional<std::string> name;

        std::optional<std::string>            static_mesh;
        std::optional<DynamicMeshDescription> dynamic_mesh;
        std::optional<std::string>            material;

        std::optional<DirectionalLight> directional_light;
        std::optional<PointLight>       point_light;
        std::optional<SpotLight>        spot_light;

        std::optional<TransformComponents> transform;
        std::optional<TransformAnimationDescription> transform_animation;
    };

    struct SceneDescription {
        std::vector<ResourceDescription> resources;
        std::vector<EntityDescription> entities;
    };

    ErrorCode add_entity_from_description(
        Entities& entities, const EntityDescription& desc);
}
