// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

// do not include manually
// used only in the composition of the matrix.hpp

#if defined(MATRIX_IMPLEMENTATION)
#  error "Martix already implemented in other file. Check includes!"
#endif

#define MATRIX_IMPLEMENTATION "default"

#include <qce/math/vector_def.hpp>

namespace QCE {
    struct matrix final {
        float x1, y1, z1, w1;
        float x2, y2, z2, w2;
        float x3, y3, z3, w3;
        float x4, y4, z4, w4;
    };

    static inline matrix matrix_init(
            float x1, float y1, float z1, float w1,
            float x2, float y2, float z2, float w2,
            float x3, float y3, float z3, float w3,
            float x4, float y4, float z4, float w4
    ) noexcept {
        return { x1, y1, z1, w1,
                 x2, y2, z2, w2,
                 x3, y3, z3, w3,
                 x4, y4, z4, w4
        };
    }

    static inline matrix matrix_init(const float* arr) noexcept {
        assert(arr);
        return { arr[0],  arr[1],  arr[2],  arr[3],
                 arr[4],  arr[5],  arr[6],  arr[7],
                 arr[8],  arr[9],  arr[10], arr[11],
                 arr[12], arr[13], arr[14], arr[15]
        };
    }

    static inline matrix matrix_init(
            const vector& v1, const vector& v2, const vector& v3, const vector& v4
    ) noexcept {
        return { v1.x, v1.y, v1.z, v1.w,
                 v2.x, v2.y, v2.z, v2.w,
                 v3.x, v3.y, v3.z, v3.w,
                 v4.x, v4.y, v4.z, v4.w
        };
    }

    static inline matrix matrix_identity() noexcept {
        return {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    static inline matrix matrix_zero() noexcept {
        return {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        };
    }

    static inline matrix operator+(const matrix& lhs, const matrix& rhs) noexcept {
        return {
            lhs.x1 + rhs.x1, lhs.y1 + rhs.y1, lhs.z1 + rhs.z1, lhs.w1 + rhs.w1,
            lhs.x2 + rhs.x2, lhs.y2 + rhs.y2, lhs.z2 + rhs.z2, lhs.w2 + rhs.w2,
            lhs.x3 + rhs.x3, lhs.y3 + rhs.y3, lhs.z3 + rhs.z3, lhs.w3 + rhs.w3,
            lhs.x4 + rhs.x4, lhs.y4 + rhs.y4, lhs.z4 + rhs.z4, lhs.w4 + rhs.w4,
        };
    }

    static inline matrix& operator+=(matrix& lhs, const matrix& rhs) noexcept {
        lhs = lhs + rhs;
        return lhs;
    }

    static inline matrix operator-(const matrix& lhs, const matrix& rhs) noexcept {
        return {
            lhs.x1 - rhs.x1, lhs.y1 - rhs.y1, lhs.z1 - rhs.z1, lhs.w1 - rhs.w1,
            lhs.x2 - rhs.x2, lhs.y2 - rhs.y2, lhs.z2 - rhs.z2, lhs.w2 - rhs.w2,
            lhs.x3 - rhs.x3, lhs.y3 - rhs.y3, lhs.z3 - rhs.z3, lhs.w3 - rhs.w3,
            lhs.x4 - rhs.x4, lhs.y4 - rhs.y4, lhs.z4 - rhs.z4, lhs.w4 - rhs.w4,
        };
    }

    static inline matrix& operator-=(matrix& lhs, const matrix& rhs) noexcept {
        lhs = lhs - rhs;
        return lhs;
    }

    static inline matrix operator*(const matrix& lhs, const matrix& rhs) noexcept {
        auto v1 = vector_init(lhs.x1, lhs.y1, lhs.z1, lhs.w1);
        auto v2 = vector_init(lhs.x2, lhs.y2, lhs.z2, lhs.w2);
        auto v3 = vector_init(lhs.x3, lhs.y3, lhs.z3, lhs.w3);
        auto v4 = vector_init(lhs.x4, lhs.y4, lhs.z4, lhs.w4);

        auto vx = vector_init(rhs.x1, rhs.x2, rhs.x3, rhs.x4);
        auto vy = vector_init(rhs.y1, rhs.y2, rhs.y3, rhs.y4);
        auto vz = vector_init(rhs.z1, rhs.z2, rhs.z3, rhs.z4);
        auto vw = vector_init(rhs.w1, rhs.w2, rhs.w3, rhs.w4);

        return {
            .x1 = vector_dot_product(v1, vx),
            .y1 = vector_dot_product(v1, vy),
            .z1 = vector_dot_product(v1, vz),
            .w1 = vector_dot_product(v1, vw),

            .x2 = vector_dot_product(v2, vx),
            .y2 = vector_dot_product(v2, vy),
            .z2 = vector_dot_product(v2, vz),
            .w2 = vector_dot_product(v2, vw),

            .x3 = vector_dot_product(v3, vx),
            .y3 = vector_dot_product(v3, vy),
            .z3 = vector_dot_product(v3, vz),
            .w3 = vector_dot_product(v3, vw),

            .x4 = vector_dot_product(v4, vx),
            .y4 = vector_dot_product(v4, vy),
            .z4 = vector_dot_product(v4, vz),
            .w4 = vector_dot_product(v4, vw)
        };
    }

    static inline matrix& operator*=(matrix& lhs, const matrix& rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }

    static inline vector operator*(const vector& lhs, const matrix& rhs) noexcept {
        auto vx = vector_init(rhs.x1, rhs.x2, rhs.x3, rhs.x4);
        auto vy = vector_init(rhs.y1, rhs.y2, rhs.y3, rhs.y4);
        auto vz = vector_init(rhs.z1, rhs.z2, rhs.z3, rhs.z4);
        auto vw = vector_init(rhs.w1, rhs.w2, rhs.w3, rhs.w4);

        return vector_init(
            vector_dot_product(lhs, vx),
            vector_dot_product(lhs, vy),
            vector_dot_product(lhs, vz),
            vector_dot_product(lhs, vw)
        );
    }

    static inline vector& operator*=(vector& lhs, const matrix& rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }

    static inline vector operator*(const matrix& lhs, const vector& rhs) noexcept {
        auto v1 = vector_init(lhs.x1, lhs.y1, lhs.z1, lhs.w1);
        auto v2 = vector_init(lhs.x2, lhs.y2, lhs.z2, lhs.w2);
        auto v3 = vector_init(lhs.x3, lhs.y3, lhs.z3, lhs.w3);
        auto v4 = vector_init(lhs.x4, lhs.y4, lhs.z4, lhs.w4);

        return vector_init(
            vector_dot_product(v1, rhs),
            vector_dot_product(v2, rhs),
            vector_dot_product(v3, rhs),
            vector_dot_product(v4, rhs)
        );
    }

    static inline void matrix_copy(const matrix& value, float* dst) noexcept {
        assert(dst);
        dst[0]  = value.x1; dst[1]  = value.y1; dst[2]  = value.z1; dst[3]  = value.w1;
        dst[4]  = value.x2; dst[5]  = value.y2; dst[6]  = value.z2; dst[7]  = value.w2;
        dst[8]  = value.x3; dst[9]  = value.y3; dst[10] = value.z3; dst[11] = value.w3;
        dst[12] = value.x4; dst[13] = value.y4; dst[14] = value.z4; dst[15] = value.w4;
    }

    static inline matrix matrix_transpose(const matrix& mtx) noexcept {
        return {
            mtx.x1, mtx.x2, mtx.x3, mtx.x4,
            mtx.y1, mtx.y2, mtx.y3, mtx.y4,
            mtx.z1, mtx.z2, mtx.z3, mtx.z4,
            mtx.w1, mtx.w2, mtx.w3, mtx.w4
        };
    }

    static inline float matrix_determinant(const matrix& mtx) noexcept {
        // TODO
        return 0.0f;
    }

    static inline matrix matrix_inverse(const matrix& mtx) noexcept {
        // TODO
        return {};
    }
}
