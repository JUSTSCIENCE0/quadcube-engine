// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>
#include <qce/objects/index.hpp>

#include <string>
#include <vector>
#include <utility>
#include <optional>

namespace QCE {
    struct Mesh {
        explicit Mesh(
                std::string           id,
                std::vector<vertex>   vertices = {},
                std::vector<index_t>  indices = {}) :
            id(std::move(id)),
            m_vertices(std::move(vertices)),
            m_indices(std::move(indices)) {}

        Mesh(const Mesh& other) = delete;
        Mesh(Mesh&& other) = delete;
        Mesh& operator=(const Mesh& other) = delete;
        Mesh& operator=(Mesh&& other) = delete;

        const std::string    id{};
        std::vector<vertex>  m_vertices{};
        std::vector<index_t> m_indices{};

        std::optional<size_t> m_render_unit_index;
    };
}
