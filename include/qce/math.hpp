// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/vector.hpp>
#include <qce/matrix.hpp>

namespace QCE {
    struct coordinate {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    struct vertex {
        coordinate position{};
    };

    struct alignas(REQUIRED_ALIGNAS) transform {
        // aligned
        vector rotation = vector_zero(); // TODO: quaternion

        // not aligned
        coordinate position{};
        coordinate scale{};
    };
}
