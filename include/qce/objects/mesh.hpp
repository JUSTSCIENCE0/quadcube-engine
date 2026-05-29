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
    struct MeshComponent {
        size_t index{};
        int    dirty_frames = 1;
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
