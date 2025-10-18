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
    struct alignas(REQUIRED_ALIGNAS) matrix final {
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
            _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
            _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
            _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
            _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
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

    static inline matrix VECTOR_CALL operator+(matrix lhs, const matrix& rhs) noexcept {
        return {
            _mm_add_ps(lhs.v1, rhs.v1),
            _mm_add_ps(lhs.v2, rhs.v2),
            _mm_add_ps(lhs.v3, rhs.v3),
            _mm_add_ps(lhs.v4, rhs.v4),
        };
    }

    static inline matrix& VECTOR_CALL operator+=(matrix& lhs, const matrix& rhs) noexcept {
        lhs = lhs + rhs;
        return lhs;
    }

    static inline matrix VECTOR_CALL operator-(matrix lhs, const matrix& rhs) noexcept {
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

    static inline vector VECTOR_CALL matrix_vector_mul(matrix lhs, vector rhs) noexcept {
        lhs.v1 = _mm_mul_ps(rhs, lhs.v1);
        lhs.v2 = _mm_mul_ps(rhs, lhs.v2);
        lhs.v3 = _mm_mul_ps(rhs, lhs.v3);
        lhs.v4 = _mm_mul_ps(rhs, lhs.v4);
        lhs = matrix_transpose(lhs);
        return _mm_add_ps(_mm_add_ps(lhs.v1, lhs.v2),
            _mm_add_ps(lhs.v3, lhs.v4));
    }

    static inline matrix VECTOR_CALL matrix_mul(matrix lhs, const matrix& rhs) noexcept {
        auto rhst = matrix_transpose(rhs);

        lhs.v1 = matrix_vector_mul(rhst, lhs.v1);
        lhs.v2 = matrix_vector_mul(rhst, lhs.v2);
        lhs.v3 = matrix_vector_mul(rhst, lhs.v3);
        lhs.v4 = matrix_vector_mul(rhst, lhs.v4);

        return lhs;
    }

    static inline vector VECTOR_CALL vector_matrix_mul(vector lhs, matrix rhs) noexcept {
        rhs = matrix_transpose(rhs);
        return matrix_vector_mul(rhs, lhs);
    }

    static inline float VECTOR_CALL matrix_determinant(matrix m) noexcept {
        auto row2 = _mm_shuffle_ps(m.v2, m.v2, _MM_SHUFFLE(2, 0, 0, 1)); // y x x z
        auto row3 = _mm_shuffle_ps(m.v3, m.v3, _MM_SHUFFLE(1, 1, 3, 2)); // z w y y
        auto row4 = _mm_shuffle_ps(m.v4, m.v4, _MM_SHUFFLE(0, 3, 2, 3)); // w z w x
        auto res1 = _mm_mul_ps(m.v1, row2);
        res1 = _mm_mul_ps(res1, row3);
        res1 = _mm_mul_ps(res1, row4);

        row2 = _mm_shuffle_ps(m.v2, m.v2, _MM_SHUFFLE(0, 1, 2, 2)); // z z y x
        row3 = _mm_shuffle_ps(m.v3, m.v3, _MM_SHUFFLE(2, 3, 0, 3)); // w x w z
        row4 = _mm_shuffle_ps(m.v4, m.v4, _MM_SHUFFLE(1, 0, 3, 1)); // y w x y
        auto mul = _mm_mul_ps(m.v1, row2);
        mul = _mm_mul_ps(mul, row3);
        mul = _mm_mul_ps(mul, row4);
        res1 = _mm_add_ps(res1, mul);

        row2 = _mm_shuffle_ps(m.v2, m.v2, _MM_SHUFFLE(1, 3, 3, 3)); // w w w y
        row3 = _mm_shuffle_ps(m.v3, m.v3, _MM_SHUFFLE(0, 0, 2, 1)); // y z x x
        row4 = _mm_shuffle_ps(m.v4, m.v4, _MM_SHUFFLE(2, 1, 0, 2)); // z x y z
        mul = _mm_mul_ps(m.v1, row2);
        mul = _mm_mul_ps(mul, row3);
        mul = _mm_mul_ps(mul, row4);
        res1 = _mm_add_ps(res1, mul);

        row2 = _mm_shuffle_ps(m.v2, m.v2, _MM_SHUFFLE(0, 0, 0, 1)); // y x x x
        row3 = _mm_shuffle_ps(m.v3, m.v3, _MM_SHUFFLE(1, 3, 2, 3)); // w z w y
        row4 = _mm_shuffle_ps(m.v4, m.v4, _MM_SHUFFLE(2, 1, 3, 2)); // z w y z
        auto res2 = _mm_mul_ps(m.v1, row2);
        res2 = _mm_mul_ps(res2, row3);
        res2 = _mm_mul_ps(res2, row4);

        row2 = _mm_shuffle_ps(m.v2, m.v2, _MM_SHUFFLE(1, 1, 2, 2)); // z z y y
        row3 = _mm_shuffle_ps(m.v3, m.v3, _MM_SHUFFLE(2, 0, 3, 1)); // y w x z
        row4 = _mm_shuffle_ps(m.v4, m.v4, _MM_SHUFFLE(0, 3, 0, 3)); // w x w x
        mul = _mm_mul_ps(m.v1, row2);
        mul = _mm_mul_ps(mul, row3);
        mul = _mm_mul_ps(mul, row4);
        res2 = _mm_add_ps(res2, mul);

        row2 = _mm_shuffle_ps(m.v2, m.v2, _MM_SHUFFLE(2, 3, 3, 3)); // w w w z
        row3 = _mm_shuffle_ps(m.v3, m.v3, _MM_SHUFFLE(0, 1, 0, 2)); // z x y x
        row4 = _mm_shuffle_ps(m.v4, m.v4, _MM_SHUFFLE(1, 0, 2, 1)); // y z x y
        mul = _mm_mul_ps(m.v1, row2);
        mul = _mm_mul_ps(mul, row3);
        mul = _mm_mul_ps(mul, row4);
        res2 = _mm_add_ps(res2, mul);

        res1 = _mm_sub_ps(res1, res2);
        mul = _mm_shuffle_ps(res1, res1, _MM_SHUFFLE(2, 3, 0, 1));
        res1 = _mm_add_ps(res1, mul);
        mul = _mm_movehl_ps(mul, res1);
        res1 = _mm_add_ss(res1, mul);
        return _mm_cvtss_f32(res1);
    }

    static inline matrix VECTOR_CALL matrix_inverse(matrix m, float det) noexcept {
        assert(0.0f != det);

        auto mt = matrix_transpose(m);

        auto tmp00 = _mm_shuffle_ps(mt.v1, mt.v1, _MM_SHUFFLE(0, 0, 0, 1)); // x 1112
        auto tmp01 = _mm_shuffle_ps(mt.v1, mt.v1, _MM_SHUFFLE(1, 1, 2, 2)); // x 2233
        auto tmp02 = _mm_shuffle_ps(mt.v1, mt.v1, _MM_SHUFFLE(2, 3, 3, 3)); // x 3444
        auto tmp03 = _mm_shuffle_ps(mt.v2, mt.v2, _MM_SHUFFLE(0, 0, 0, 1)); // y 1112
        auto tmp04 = _mm_shuffle_ps(mt.v2, mt.v2, _MM_SHUFFLE(1, 1, 2, 2)); // y 2233
        auto tmp05 = _mm_shuffle_ps(mt.v2, mt.v2, _MM_SHUFFLE(2, 3, 3, 3)); // y 3444
        auto tmp06 = _mm_shuffle_ps(mt.v3, mt.v3, _MM_SHUFFLE(0, 0, 0, 1)); // z 1112
        auto tmp07 = _mm_shuffle_ps(mt.v3, mt.v3, _MM_SHUFFLE(1, 1, 2, 2)); // z 2233
        auto tmp08 = _mm_shuffle_ps(mt.v3, mt.v3, _MM_SHUFFLE(2, 3, 3, 3)); // z 3444

        mt.v1 = _mm_shuffle_ps(mt.v4, mt.v4, _MM_SHUFFLE(0, 0, 0, 1)); // w 1112
        mt.v2 = _mm_shuffle_ps(mt.v4, mt.v4, _MM_SHUFFLE(1, 1, 2, 2)); // w 2233
        mt.v3 = _mm_shuffle_ps(mt.v4, mt.v4, _MM_SHUFFLE(2, 3, 3, 3)); // w 3444

        m.v2 = _mm_sub_ps(_mm_mul_ps(tmp07, mt.v3), _mm_mul_ps(tmp08, mt.v2));
        m.v3 = _mm_sub_ps(_mm_mul_ps(tmp06, mt.v3), _mm_mul_ps(tmp08, mt.v1));
        m.v4 = _mm_sub_ps(_mm_mul_ps(tmp06, mt.v2), _mm_mul_ps(tmp07, mt.v1));

        m.v1 = _mm_mul_ps(tmp03, m.v2);
        m.v1 = _mm_sub_ps(m.v1, _mm_mul_ps(tmp04, m.v3));
        m.v1 = _mm_add_ps(m.v1, _mm_mul_ps(tmp05, m.v4));

        m.v2 = _mm_mul_ps(tmp00, m.v2);
        m.v2 = _mm_sub_ps(m.v2, _mm_mul_ps(tmp01, m.v3));
        m.v2 = _mm_add_ps(m.v2, _mm_mul_ps(tmp02, m.v4));

        mt.v4 = _mm_sub_ps(_mm_mul_ps(tmp04, mt.v3), _mm_mul_ps(tmp05, mt.v2));
        mt.v3 = _mm_sub_ps(_mm_mul_ps(tmp03, mt.v3), _mm_mul_ps(tmp05, mt.v1));
        mt.v2 = _mm_sub_ps(_mm_mul_ps(tmp03, mt.v2), _mm_mul_ps(tmp04, mt.v1));

        m.v3 = _mm_mul_ps(tmp00, mt.v4);
        m.v3 = _mm_sub_ps(m.v3, _mm_mul_ps(tmp01, mt.v3));
        m.v3 = _mm_add_ps(m.v3, _mm_mul_ps(tmp02, mt.v2));

        mt.v1 = _mm_sub_ps(_mm_mul_ps(tmp04, tmp08), _mm_mul_ps(tmp05, tmp07));
        mt.v2 = _mm_sub_ps(_mm_mul_ps(tmp03, tmp08), _mm_mul_ps(tmp05, tmp06));
        mt.v3 = _mm_sub_ps(_mm_mul_ps(tmp03, tmp07), _mm_mul_ps(tmp04, tmp06));

        m.v4 = _mm_mul_ps(tmp00, mt.v1);
        m.v4 = _mm_sub_ps(m.v4, _mm_mul_ps(tmp01, mt.v2));
        m.v4 = _mm_add_ps(m.v4, _mm_mul_ps(tmp02, mt.v3));

        tmp07 = _mm_set_ps(-1.0f, 1.0f, -1.0f, 1.0f);
        m.v1 = _mm_mul_ps(m.v1, tmp07);
        m.v3 = _mm_mul_ps(m.v3, tmp07);
        tmp07 = _mm_set_ps(1.0f, -1.0f, 1.0f, -1.0f);
        m.v2 = _mm_mul_ps(m.v2, tmp07);
        m.v4 = _mm_mul_ps(m.v4, tmp07);

        tmp07 = _mm_set1_ps(det);
        m.v1 = _mm_div_ps(m.v1, tmp07);
        m.v2 = _mm_div_ps(m.v2, tmp07);
        m.v3 = _mm_div_ps(m.v3, tmp07);
        m.v4 = _mm_div_ps(m.v4, tmp07);

        return m;
    }

    static inline matrix VECTOR_CALL quaternion_to_rotation_matrix(vector q) noexcept {
        // TODO
        return matrix_identity();
    }
}
