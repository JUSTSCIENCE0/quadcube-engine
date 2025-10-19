// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/vector.hpp>
#include <qce/matrix.hpp>

namespace QCE {
    struct float3d {
        float arr[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        float& x() noexcept { return arr[0]; }
        float& y() noexcept { return arr[1]; }
        float& z() noexcept { return arr[2]; }

        const float& x() const noexcept { return arr[0]; }
        const float& y() const noexcept { return arr[1]; }
        const float& z() const noexcept { return arr[2]; }
    };

    struct float4d : public float3d {
        float& w() noexcept { return arr[3]; }
        const float& w() const noexcept { return arr[3]; }
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
