// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/mesh.hpp>
#include <qce/objects/texture.hpp>
#include <qce/objects/material.hpp>

namespace QCE {
    struct ModelDescription {
        std::string mesh_id{};
        std::string material_id{};
    };

    struct ModelResources {
        std::vector<Mesh>      meshes{};
        std::vector<Texture2D> textures{};
        std::vector<Material>  materials{};
    };
}
