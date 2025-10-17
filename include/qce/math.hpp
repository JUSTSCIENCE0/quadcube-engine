// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/vector.hpp>
#include <qce/matrix.hpp>

namespace QCE {
    struct float3d {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    struct float4d {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    };

    struct float4x4 {
        union {
            float arr[16];
            float4d rows[4];
        };
    };

    struct vertex {
        float3d position{};
    };
}
