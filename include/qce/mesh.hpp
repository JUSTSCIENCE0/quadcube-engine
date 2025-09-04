// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math.hpp>

#include <vector>

namespace QCE {
    struct Mesh {
        std::vector<vertex>   vertices;
        std::vector<uint32_t> indices;
    };
}
