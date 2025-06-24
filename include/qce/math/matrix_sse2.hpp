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

#define MATRIX_IMPLEMENTATION "sse2"

#include <qce/math/vector_sse2.hpp>

namespace QCE {
    struct alignas(16) matrix final {
        __m128 v1;
        __m128 v2;
        __m128 v3;
        __m128 v4;
    };

    static inline matrix VECTOR_CALL matrix_init(
            float x1, float y1, float z1, float w1,
            float x2, float y2, float z2, float w2,
            float x3, float y3, float z3, float w3,
            float x4, float y4, float z4, float w4
    ) noexcept {
        return {
            _mm_set_ps(w1, z1, y1, x1),
            _mm_set_ps(w2, z2, y2, x2),
            _mm_set_ps(w3, z3, y3, x3),
            _mm_set_ps(w4, z4, y4, x4)
        };
    }

    static inline matrix VECTOR_CALL matrix_init(const float* arr) noexcept {
        assert(arr);
        return { 
            _mm_loadu_ps(arr),
            _mm_loadu_ps(arr + 4),
            _mm_loadu_ps(arr + 8),
            _mm_loadu_ps(arr + 12)
        };
    }

    static inline matrix VECTOR_CALL matrix_init(
            vector v1, vector v2, vector v3, vector v4
    ) noexcept {
        return { v1, v2, v3, v4 };
    }

    static inline matrix VECTOR_CALL matrix_identity() noexcept {
        return {
            _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f),
            _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
            _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
            _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f)
        };
    }

    static inline matrix VECTOR_CALL matrix_zero() noexcept {
        return {
            _mm_setzero_ps(),
            _mm_setzero_ps(),
            _mm_setzero_ps(),
            _mm_setzero_ps()
        };
    }

    static inline matrix VECTOR_CALL operator+(matrix lhs, matrix rhs) noexcept {
        return {
            _mm_add_ps(lhs.v1, rhs.v1),
            _mm_add_ps(lhs.v2, rhs.v2),
            _mm_add_ps(lhs.v3, rhs.v3),
            _mm_add_ps(lhs.v4, rhs.v4),
        };
    }

    static inline matrix& VECTOR_CALL operator+=(matrix& lhs, matrix rhs) noexcept {
        lhs = lhs + rhs;
        return lhs;
    }

    static inline matrix VECTOR_CALL operator-(matrix lhs, matrix rhs) noexcept {
        return {
            _mm_sub_ps(lhs.v1, rhs.v1),
            _mm_sub_ps(lhs.v2, rhs.v2),
            _mm_sub_ps(lhs.v3, rhs.v3),
            _mm_sub_ps(lhs.v4, rhs.v4),
        };
    }

    static inline matrix& VECTOR_CALL operator-=(matrix& lhs, matrix rhs) noexcept {
        lhs = lhs - rhs;
        return lhs;
    }

    static inline void VECTOR_CALL matrix_copy(matrix value, float* dst) noexcept {
        assert(dst);
        _mm_storeu_ps(dst,      value.v1);
        _mm_storeu_ps(dst + 4,  value.v2);
        _mm_storeu_ps(dst + 8,  value.v3);
        _mm_storeu_ps(dst + 12, value.v4);
    }

    static inline matrix VECTOR_CALL matrix_mul(matrix lhs, matrix rhs) noexcept {
        // TODO
        return {
        };
    }

    static inline vector VECTOR_CALL vector_matrix_mul(vector lhs, matrix rhs) noexcept {
        // TODO
        return {
        };
    }

    static inline vector VECTOR_CALL matrix_vector_mul(matrix lhs, vector rhs) noexcept {
        // TODO
        return {
        };
    }

    static inline matrix VECTOR_CALL matrix_transpose(matrix mtx) noexcept {
        auto lo12 = _mm_unpacklo_ps(mtx.v1, mtx.v2);
        auto hi12 = _mm_unpackhi_ps(mtx.v1, mtx.v2);
        auto lo34 = _mm_unpacklo_ps(mtx.v3, mtx.v4);
        auto hi34 = _mm_unpackhi_ps(mtx.v3, mtx.v4);

        return { 
            _mm_movelh_ps(lo12, lo34),
            _mm_movehl_ps(lo34, lo12),
            _mm_movelh_ps(hi12, hi34),
            _mm_movehl_ps(hi34, hi12)
        };
    }

    static inline float VECTOR_CALL matrix_determinant(matrix m) noexcept {
        // TODO
        return {
        };
    }

    static inline matrix VECTOR_CALL matrix_inverse(matrix m, float det) noexcept {
        assert(0.0f != det);

        // TODO
        return {
        };
    }
}
