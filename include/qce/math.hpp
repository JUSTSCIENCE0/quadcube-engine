// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/vector.hpp>
#include <qce/matrix.hpp>

#include <cmath>

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

    static inline float4d rotation_matrix_to_quaternion(const float4x4& m) noexcept {
        float4d q{};

        float t = 0;

        if (m(2, 2) < 0) {
            if (m(0, 0) > m(1, 1)) {
                t = 1 + m(0, 0) - m(1, 1) - m(2, 2);
                q.x() = t;
                q.y() = m(1, 0) + m(0, 1);
                q.z() = m(0, 2) + m(2, 0);
                q.w() = m(2, 1) - m(1, 2);
            }
            else {
                t = 1 - m(0, 0) + m(1, 1) - m(2, 2);
                q.x() = m(1, 0) + m(0, 1);
                q.y() = t;
                q.z() = m(2, 1) + m(1, 2);
                q.w() = m(0, 2) - m(2, 0);
            }
        }
        else {
            if (m(0, 0) < (-1.0f * m(1, 1))) {
                t = 1 - m(0, 0) - m(1, 1) + m(2, 2);
                q.x() = m(0, 2) + m(2, 0);
                q.y() = m(2, 1) + m(1, 2);
                q.z() = t;
                q.w() = m(1, 0) - m(0, 1);
            }
            else {
                t = 1 + m(0, 0) + m(1, 1) + m(2, 2);
                q.x() = m(2, 1) - m(1, 2);
                q.y() = m(0, 2) - m(2, 0);
                q.z() = m(1, 0) - m(0, 1);
                q.w() = t;
            }
        }

        auto k = 0.5f / std::sqrtf(t);
        q.x() *= k;
        q.y() *= k;
        q.z() *= k;
        q.w() *= k;

        return q;
    }
}
