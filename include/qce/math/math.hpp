// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/vector.hpp>
#include <qce/math/matrix.hpp>

#include <cmath>

namespace QCE {
    static constexpr float PI     = 3.1415926535f;
    static constexpr float PI_DEG = 180.0f;

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
        float arr[16] = {};

        float& operator()(ptrdiff_t row, ptrdiff_t col) noexcept {
            return arr[4 * row + col];
        }
        const float& operator()(ptrdiff_t row, ptrdiff_t col) const noexcept {
            return arr[4 * row + col];
        }
    };

    struct vertex {
        float3d position{};
    };

    static inline float constexpr deg_to_rad(float deg) {
        return (deg / PI_DEG) * PI;
    }
    static inline float constexpr rad_to_deg(float rad) {
        return (rad / PI) * PI_DEG;
    }
}
