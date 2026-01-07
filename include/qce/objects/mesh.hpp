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
    };

    struct Mesh {
        std::string          id{};
        std::vector<vertex>  vertices{};
        std::vector<index_t> indices{};

        std::optional<size_t> render_unit_index = std::nullopt;
    };
}
