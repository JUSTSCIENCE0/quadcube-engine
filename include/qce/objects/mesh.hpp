// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math.hpp>
#include <qce/objects/identification.hpp>

#include <vector>
#include <utility>

namespace QCE {
    struct Mesh {
        explicit Mesh(
                std::string           name,
                std::vector<vertex>   vertices = {},
                std::vector<uint32_t> indices = {}) :
            m_id(get_unique_name(name)),
            m_vertices(std::move(vertices)),
            m_indices(std::move(indices)) {}

        Mesh(const Mesh& other) :
            m_id(get_unique_name(extract_name(other.m_id))),
            m_vertices(other.m_vertices),
            m_indices(other.m_indices) {}
        Mesh(Mesh&& other) :
            m_id(get_unique_name(extract_name(other.m_id))),
            m_vertices(std::move(other.m_vertices)),
            m_indices(std::move(other.m_indices)) {}
        Mesh& operator=(const Mesh& other) {
            if (this == &other)
                return *this;

            m_vertices = other.m_vertices;
            m_indices = other.m_indices;
            return *this;
        }
        Mesh& operator=(Mesh&& other) noexcept {
            if (this == &other)
                return *this;

            m_vertices = std::move(other.m_vertices);
            m_indices = std::move(other.m_indices);
            return *this;
        }

        const std::string     m_id{};
        std::vector<vertex>   m_vertices{};
        std::vector<uint32_t> m_indices{};
    };
}
