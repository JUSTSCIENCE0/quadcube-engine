// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>
#include <qce/objects/index.hpp>
#include <qce/objects/command.hpp>

#include <string>
#include <vector>
#include <utility>
#include <optional>

namespace QCE {
    struct StaticMesh {
        size_t index{};
    };

    struct DynamicMesh {
        size_t   base_mesh_index= 0;
        uint32_t max_vertices_count = 0;
        uint32_t max_indeces_count = 0;
        size_t   deformated_mesh_index = 0;
    };

    struct Mesh {
        Mesh() = default;
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        std::string          id{};
        std::vector<vertex>  vertices{};
        std::vector<index_t> indices{};
    };
}
