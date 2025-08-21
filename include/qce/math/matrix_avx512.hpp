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

#define MATRIX_IMPLEMENTATION "avx512"

#include <qce/math/vector_avx512.hpp>

namespace QCE {
    using matrix = __m512;

    static inline matrix VECTOR_CALL matrix_init(
        float x1, float y1, float z1, float w1,
        float x2, float y2, float z2, float w2,
        float x3, float y3, float z3, float w3,
        float x4, float y4, float z4, float w4
    ) noexcept {
        return _mm512_set_ps(
            w4, z4, y4, x4,
            w3, z3, y3, x3,
            w2, z2, y2, x2,
            w1, z1, y1, x1
        );
    }

    static inline matrix VECTOR_CALL matrix_init(const float* arr) noexcept {
        assert(arr);
        return _mm512_loadu_ps(arr);
    }

    static inline matrix VECTOR_CALL matrix_init(
        vector v1, vector v2, vector v3, vector v4
    ) noexcept {
        __m512 r = _mm512_undefined_ps();
        r = _mm512_insertf32x4(r, _mm512_castps512_ps128(v1), 0);
        r = _mm512_insertf32x4(r, _mm512_castps512_ps128(v2), 1);
        r = _mm512_insertf32x4(r, _mm512_castps512_ps128(v3), 2);
        r = _mm512_insertf32x4(r, _mm512_castps512_ps128(v4), 3);
        return r;
    }

    static inline matrix VECTOR_CALL matrix_identity() noexcept {
        return _mm512_set_ps(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    static inline matrix VECTOR_CALL matrix_zero() noexcept {
        return _mm512_setzero_ps();
    }

    static inline void VECTOR_CALL matrix_copy(matrix value, float* dst) noexcept {
        assert(dst);
        _mm512_storeu_ps(dst, value);
    }

    static inline matrix VECTOR_CALL matrix_transpose(matrix mtx) noexcept {
        const auto mask = _mm512_set_epi32(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
        return _mm512_permutexvar_ps(mask, mtx);
    }

    namespace PrivateImplementation {
        static inline __m512 VECTOR_CALL vector_X_matrix(__m128 v4, __m512 m16) {
            auto col = _mm512_broadcast_f32x4(v4);
            m16 = _mm512_mul_ps(col, m16);
            col = _mm512_shuffle_ps(m16, m16, _MM_SHUFFLE(2, 3, 0, 1));
            m16 = _mm512_add_ps(m16, col);
            col = _mm512_shuffle_ps(m16, m16, _MM_SHUFFLE(0, 1, 2, 3));
            return _mm512_add_ps(m16, col);
        }
    }

    static inline matrix VECTOR_CALL matrix_mul(matrix lhs, matrix rhs) noexcept {
        using namespace PrivateImplementation;

        rhs = matrix_transpose(rhs);

        auto res = vector_X_matrix(_mm512_castps512_ps128(rhs), lhs);

        auto mul = vector_X_matrix(_mm512_extractf32x4_ps(rhs, 1), lhs);
        res = _mm512_mask_blend_ps(0b0010001000100010, res, mul);

        mul = vector_X_matrix(_mm512_extractf32x4_ps(rhs, 2), lhs);
        res = _mm512_mask_blend_ps(0b0100010001000100, res, mul);

        mul = vector_X_matrix(_mm512_extractf32x4_ps(rhs, 3), lhs);
        res = _mm512_mask_blend_ps(0b1000100010001000, res, mul);

        return res;
    }

    static inline vector VECTOR_CALL matrix_vector_mul(matrix lhs, vector rhs) noexcept {
        lhs = PrivateImplementation::vector_X_matrix(_mm512_castps512_ps128(rhs), lhs);

        const auto mask = _mm512_set_epi32(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 8, 4, 0);
        rhs = _mm512_maskz_permutexvar_ps(0b1111, mask, lhs);
        return rhs;
    }

    static inline vector VECTOR_CALL vector_matrix_mul(vector lhs, matrix rhs) noexcept {
        rhs = matrix_transpose(rhs);
        return matrix_vector_mul(rhs, lhs);
    }

    static inline float VECTOR_CALL matrix_determinant(matrix m) noexcept {
        auto v1 = _mm512_broadcast_f32x4(_mm512_castps512_ps128(m));
        auto v2 = _mm512_permutexvar_ps(
            _mm512_set_epi32(6, 7, 7, 7, 4, 4, 4, 5, 5, 7, 7, 7, 6, 4, 4, 5), m);
        auto v3 = _mm512_permutexvar_ps(
            _mm512_set_epi32(8, 9, 8, 10, 9, 11, 10, 11, 8, 8, 10, 9, 9, 9, 11, 10), m);
        auto v4 = _mm512_permutexvar_ps(
            _mm512_set_epi32(13, 12, 14, 13, 14, 13, 15, 14, 14, 13, 12, 14, 12, 15, 14, 15), m);
        auto mul1 = _mm512_mul_ps(v1, v2);
        mul1 = _mm512_mul_ps(mul1, v3);
        mul1 = _mm512_mul_ps(mul1, v4);

        v1 = _mm512_maskz_permutexvar_ps(0b0000111100001111,
            _mm512_set_epi32(0, 0, 0, 0, 3, 2, 1, 0, 0, 0, 0, 0, 3, 2, 1, 0), m);
        v2 = _mm512_maskz_permutexvar_ps(0b0000111100001111,
            _mm512_set_epi32(0, 0, 0, 0, 5, 5, 6, 6, 0, 0, 0, 0, 4, 5, 6, 6), m);
        v3 = _mm512_maskz_permutexvar_ps(0b0000111100001111,
            _mm512_set_epi32(0, 0, 0, 0, 10, 8, 11, 9, 0, 0, 0, 0, 10, 11, 8, 11), m);
        v4 = _mm512_maskz_permutexvar_ps(0b0000111100001111,
            _mm512_set_epi32(0, 0, 0, 0, 12, 15, 12, 15, 0, 0, 0, 0, 13, 12, 15, 13), m);
        auto mul2 = _mm512_mul_ps(v1, v2);
        mul2 = _mm512_mul_ps(mul2, v3);
        mul2 = _mm512_mul_ps(mul2, v4);

        auto sum = _mm512_add_ps(mul1, mul2);
        auto shuf = _mm512_shuffle_f32x4(sum, sum, _MM_SHUFFLE(2, 3, 0, 1));
        sum = _mm512_add_ps(sum, shuf);
        shuf = _mm512_shuffle_f32x4(sum, sum, _MM_SHUFFLE(0, 1, 2, 3));
        sum = _mm512_sub_ps(sum, shuf);
        
        shuf = _mm512_movehdup_ps(sum);
        sum = _mm512_add_ps(sum, shuf);
        shuf = _mm512_unpackhi_ps(sum, sum);
        sum = _mm512_add_ps(sum, shuf);
        return _mm_cvtss_f32(_mm512_castps512_ps128(sum));
    }

    static inline matrix VECTOR_CALL matrix_inverse(matrix m, float det) noexcept {
        assert(0.0f != det);

        auto tmp00 = _mm512_permutexvar_ps(
            _mm512_set_epi32(5, 5, 9, 9, 5, 5, 9, 9, 6, 6, 10, 10, 6, 6, 10, 10), m);
        auto tmp01 = _mm512_permutexvar_ps(
            _mm512_set_epi32(10, 14, 14, 14, 11, 15, 15, 15, 11, 15, 15, 15, 11, 15, 15, 15), m);
        auto tmp02 = _mm512_permutexvar_ps(
            _mm512_set_epi32(9, 13, 13, 13, 9, 13, 13, 13, 10, 14, 14, 14, 10, 14, 14, 14), m);
        auto tmp03 = _mm512_permutexvar_ps(
            _mm512_set_epi32(6, 6, 10, 10, 7, 7, 11, 11, 7, 7, 11, 11, 7, 7, 11, 11), m);
        auto tmp04 = _mm512_permutexvar_ps(
            _mm512_set_epi32(1, 1, 1, 5, 1, 1, 1, 5, 2, 2, 2, 6, 2, 2, 2, 6), m);
        auto tmp05 = _mm512_permutexvar_ps(
            _mm512_set_epi32(2, 2, 2, 6, 3, 3, 3, 7, 3, 3, 3, 7, 3, 3, 3, 7), m);

        auto mul0 = _mm512_permutexvar_ps(
            _mm512_set_epi32(0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 1, 1, 1, 5), m);
        auto mul1 = _mm512_sub_ps(_mm512_mul_ps(tmp00, tmp01), _mm512_mul_ps(tmp02, tmp03));
        auto res = _mm512_mul_ps(mul0, mul1);

        mul0 = _mm512_permutexvar_ps(
            _mm512_set_epi32(4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 8, 8, 5, 5, 9, 9), m);
        mul1 = _mm512_sub_ps(_mm512_mul_ps(tmp04, tmp01), _mm512_mul_ps(tmp05, tmp02));
        res = _mm512_sub_ps(res, _mm512_mul_ps(mul0, mul1));

        mul0 = _mm512_permutexvar_ps(
            _mm512_set_epi32(8, 12, 12, 12, 8, 12, 12, 12, 8, 12, 12, 12, 9, 13, 13, 13), m);
        mul1 = _mm512_sub_ps(_mm512_mul_ps(tmp04, tmp03), _mm512_mul_ps(tmp00, tmp05));
        res = _mm512_add_ps(res, _mm512_mul_ps(mul0, mul1));

        mul0 = _mm512_set_ps(
             1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,  1.0f
        );
        res = _mm512_mul_ps(res, mul0);

        mul0 = _mm512_set1_ps(det);
        res = _mm512_div_ps(res, mul0);

        return res;
    }
}
