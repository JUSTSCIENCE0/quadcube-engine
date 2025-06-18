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

    static inline matrix& operator+=(      matrix& lhs, const matrix& rhs) noexcept;
    static inline matrix  operator+ (const matrix& lhs, const matrix& rhs) noexcept;
    static inline matrix& operator-=(      matrix& lhs, const matrix& rhs) noexcept;
    static inline matrix  operator- (const matrix& lhs, const matrix& rhs) noexcept;

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
        return {};
    }
}
