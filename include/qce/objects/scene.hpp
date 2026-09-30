// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/texture.hpp>
#include <qce/objects/figures.hpp>
#include <qce/objects/material.hpp>
#include <qce/objects/animation.hpp>

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

    struct SceneDescription {
        std::vector<ResourceDescription> resources;
    };
}
