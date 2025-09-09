// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math.hpp>

#include <string>
#include <vector>
#include <utility>

namespace QCE {
    struct Mesh {
        explicit Mesh(
                std::string           name,
                std::vector<vertex>   vertices = {},
                std::vector<uint32_t> indices = {}) :
            m_id(std::move(name)),
            m_vertices(std::move(vertices)),
            m_indices(std::move(indices)) {}

        Mesh(const Mesh& other) = delete;
        Mesh(Mesh&& other) noexcept = default;
        Mesh& operator=(const Mesh& other) = delete;
        Mesh& operator=(Mesh&& other) noexcept = default;

        const std::string     m_id{};
        std::vector<vertex>   m_vertices{};
        std::vector<uint32_t> m_indices{};
    };
}
