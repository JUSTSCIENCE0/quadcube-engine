// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/vector.hpp>
#include <qce/matrix.hpp>

namespace QCE {
    struct coordinate3d {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    struct vertex {
        coordinate3d position{};
    };

    struct alignas(REQUIRED_ALIGNAS) transform {
        // aligned
        vector rotation = vector_zero(); // TODO: quaternion

        // not aligned
        coordinate3d position{};
        coordinate3d scale = { 1.0f, 1.0f, 1.0f };
    };
}
