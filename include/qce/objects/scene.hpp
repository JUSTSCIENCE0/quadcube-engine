// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/texture.hpp>
#include <qce/objects/figures.hpp>
#include <qce/objects/material.hpp>
#include <qce/objects/animation.hpp>

#include <qce/components/transform.hpp>
#include <qce/components/light.hpp>

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

    struct TransformAnimationDescription {
        std::string animation_name;
        bool is_looped = false;
    };

    struct EntityDescription {
        std::optional<std::string> name;

        std::optional<std::string> static_mesh;
        std::optional<std::string> dynamic_mesh;
        std::optional<std::string> material;

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
}
