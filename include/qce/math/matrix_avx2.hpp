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

#define MATRIX_IMPLEMENTATION "avx2"

#include <qce/math/vector_avx2.hpp>

namespace QCE {
    struct alignas(REQUIRED_ALIGNAS) matrix final {
        __m256 v34;
        __m256 v12;
    };

    static inline matrix VECTOR_CALL matrix_init(
        float x1, float y1, float z1, float w1,
        float x2, float y2, float z2, float w2,
        float x3, float y3, float z3, float w3,
        float x4, float y4, float z4, float w4
    ) noexcept {
        return {
            .v34 = _mm256_set_ps(
                        w4, z4, y4, x4,
                        w3, z3, y3, x3),
            .v12 = _mm256_set_ps(
                        w2, z2, y2, x2,
                        w1, z1, y1, x1)

        };
    }

    static inline matrix VECTOR_CALL matrix_init(const float* arr) noexcept {
        assert(arr);
        return {
            .v34 = _mm256_loadu_ps(arr + 8),
            .v12 = _mm256_loadu_ps(arr)
        };
    }

    static inline matrix VECTOR_CALL matrix_init(
        vector v1, vector v2, vector v3, vector v4
    ) noexcept {
        return {
            .v34 = _mm256_insertf128_ps(v3, _mm256_castps256_ps128(v4), 1),
            .v12 = _mm256_insertf128_ps(v1, _mm256_castps256_ps128(v2), 1),
        };
    }

    static inline matrix VECTOR_CALL matrix_identity() noexcept {
        return {
            .v34 = _mm256_set_ps(
                        1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f),
            .v12 = _mm256_set_ps(
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f)
        };
    }

    static inline matrix VECTOR_CALL matrix_zero() noexcept {
        return {
            .v34 = _mm256_setzero_ps(),
            .v12 = _mm256_setzero_ps()
        };
    }

    static inline matrix VECTOR_CALL operator+(matrix lhs, const matrix& rhs) noexcept {
        return {
            .v34 = _mm256_add_ps(lhs.v34, rhs.v34),
            .v12 = _mm256_add_ps(lhs.v12, rhs.v12)
        };
    }

    static inline matrix& VECTOR_CALL operator+=(matrix& lhs, const matrix& rhs) noexcept {
        lhs = lhs + rhs;
        return lhs;
    }

    static inline matrix VECTOR_CALL operator-(matrix lhs, const matrix& rhs) noexcept {
        return {
            .v34 = _mm256_sub_ps(lhs.v34, rhs.v34),
            .v12 = _mm256_sub_ps(lhs.v12, rhs.v12)
        };
    }

    static inline matrix& VECTOR_CALL operator-=(matrix& lhs, matrix rhs) noexcept {
        lhs = lhs - rhs;
        return lhs;
    }

    static inline void VECTOR_CALL matrix_copy(matrix value, float* dst) noexcept {
        assert(dst);
        _mm256_storeu_ps(dst, value.v12);
        _mm256_storeu_ps(dst + 8, value.v34);
    }

    static inline matrix VECTOR_CALL matrix_transpose(matrix mtx) noexcept {
        // AVX
        // mtx.v12 = _mm256_permute_ps(mtx.v12, _MM_SHUFFLE(3, 1, 2, 0));
        // mtx.v34 = _mm256_permute_ps(mtx.v34, _MM_SHUFFLE(3, 1, 2, 0));
        //
        // auto xz1324 = _mm256_unpacklo_ps(mtx.v12, mtx.v34);
        // auto yw1324 = _mm256_unpackhi_ps(mtx.v12, mtx.v34);
        //
        // auto xzyw13 = _mm256_permute2f128_ps(xz1324, yw1324, 0x20);
        // auto xzyw24 = _mm256_permute2f128_ps(xz1324, yw1324, 0x31);
        //
        // mtx.v12 = _mm256_unpacklo_ps(xzyw13, xzyw24);
        // mtx.v34 = _mm256_unpackhi_ps(xzyw13, xzyw24);

        // AVX2
        auto xy1324 = _mm256_unpacklo_ps(mtx.v12, mtx.v34);
        auto zw1324 = _mm256_unpackhi_ps(mtx.v12, mtx.v34);
        const auto mask = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);
        mtx.v12 = _mm256_permutevar8x32_ps(xy1324, mask);
        mtx.v34 = _mm256_permutevar8x32_ps(zw1324, mask);

        return mtx;
    }

    namespace PrivateImplementation {
        static inline __m256 VECTOR_CALL vector256_part_mul(__m256 lhs, __m256 rhs) {
            auto mul = _mm256_mul_ps(lhs, rhs);
            auto shf = _mm256_shuffle_ps(mul, mul, _MM_SHUFFLE(2, 3, 0, 1));
            mul = _mm256_add_ps(mul, shf);
            shf = _mm256_shuffle_ps(mul, mul, _MM_SHUFFLE(0, 1, 2, 3));
            return _mm256_add_ps(mul, shf);
        }
    }

    static inline matrix VECTOR_CALL matrix_mul(matrix lhs, matrix rhs) noexcept {
        using namespace PrivateImplementation;

        rhs = matrix_transpose(rhs);

        auto row = _mm256_permute2f128_ps(rhs.v12, rhs.v12, 0x20);
        auto res12 = vector256_part_mul(row, lhs.v12);
        auto res34 = vector256_part_mul(row, lhs.v34);

        row = _mm256_permute2f128_ps(rhs.v12, rhs.v12, 0x31);
        auto mul1 = vector256_part_mul(row, lhs.v12);
        res12 = _mm256_blend_ps(res12, mul1, 0b00100010);
        auto mul2 = vector256_part_mul(row, lhs.v34);
        res34 = _mm256_blend_ps(res34, mul2, 0b00100010);

        row = _mm256_permute2f128_ps(rhs.v34, rhs.v34, 0x20);
        mul1 = vector256_part_mul(row, lhs.v12);
        res12 = _mm256_blend_ps(res12, mul1, 0b01000100);
        mul2 = vector256_part_mul(row, lhs.v34);
        res34 = _mm256_blend_ps(res34, mul2, 0b01000100);

        row = _mm256_permute2f128_ps(rhs.v34, rhs.v34, 0x31);
        mul1 = vector256_part_mul(row, lhs.v12);
        res12 = _mm256_blend_ps(res12, mul1, 0b10001000);
        mul2 = vector256_part_mul(row, lhs.v34);
        res34 = _mm256_blend_ps(res34, mul2, 0b10001000);

        return { .v34 = res34, .v12 = res12 };
    }

    static inline vector VECTOR_CALL matrix_vector_mul(matrix lhs, vector rhs) noexcept {
        // AVX
        // auto vec = _mm256_permute2f128_ps(rhs, rhs, 0x20);
        //
        // lhs.v12 = _mm256_mul_ps(lhs.v12, vec);
        // lhs.v34 = _mm256_mul_ps(lhs.v34, vec);
        //
        // auto shf1 = _mm256_shuffle_ps(lhs.v12, lhs.v34, _MM_SHUFFLE(2, 0, 2, 0));
        // auto shf2 = _mm256_shuffle_ps(lhs.v12, lhs.v34, _MM_SHUFFLE(3, 1, 3, 1));
        // auto sum = _mm256_add_ps(shf1, shf2);
        // shf1 = _mm256_shuffle_ps(sum, sum, _MM_SHUFFLE(2, 3, 0, 1));
        // sum = _mm256_add_ps(sum, shf1);
        // shf1 = _mm256_permute2f128_ps(sum, sum, 0x31);
        //
        // rhs = _mm256_blend_ps(sum, shf1, 0b10101010);
        // return _mm256_permute2f128_ps(rhs, _mm256_setzero_ps(), 0x20);

        // AVX2
        using namespace PrivateImplementation;
        auto vec = _mm256_permute2f128_ps(rhs, rhs, 0x20);
        auto mul12 = vector256_part_mul(vec, lhs.v12);
        auto mul34 = vector256_part_mul(vec, lhs.v34);
        mul12 = _mm256_unpacklo_ps(mul12, mul34);
        mul12 = _mm256_permutevar8x32_ps(mul12,
            _mm256_set_epi32(0, 0, 0, 0, 5, 1, 4, 0));

        return _mm256_permute2f128_ps(mul12, _mm256_setzero_ps(), 0x20);
    }

    static inline vector VECTOR_CALL vector_matrix_mul(vector lhs, matrix rhs) noexcept {
        rhs = matrix_transpose(rhs);
        return matrix_vector_mul(rhs, lhs);
    }

    static inline float VECTOR_CALL matrix_determinant(matrix m) noexcept {
        auto v1 = _mm256_insertf128_ps(m.v12, _mm256_castps256_ps128(m.v12), 1);
        auto v2 = _mm256_permutevar8x32_ps(m.v12,
            _mm256_set_epi32(4, 4, 4, 5, 6, 4, 4, 5));
        auto v3 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(1, 3, 2, 3, 1, 1, 3, 2));
        auto v4 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(6, 5, 7, 6, 4, 7, 6, 7));

        auto mul14 = _mm256_mul_ps(v1, v2);
        mul14 = _mm256_mul_ps(mul14, v3);
        mul14 = _mm256_mul_ps(mul14, v4);

        v2 = _mm256_permutevar8x32_ps(m.v12,
            _mm256_set_epi32(5, 5, 6, 6, 4, 5, 6, 6));
        v3 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(2, 0, 3, 1, 2, 3, 0, 3));
        v4 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(4, 7, 4, 7, 5, 4, 7, 5));

        auto mul25 = _mm256_mul_ps(v1, v2);
        mul25 = _mm256_mul_ps(mul25, v3);
        mul25 = _mm256_mul_ps(mul25, v4);

        v2 = _mm256_permutevar8x32_ps(m.v12,
            _mm256_set_epi32(6, 7, 7, 7, 5, 7, 7, 7));
        v3 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(0, 1, 0, 2, 0, 0, 2, 1));
        v4 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(5, 4, 6, 5, 6, 5, 4, 6));

        auto mul36 = _mm256_mul_ps(v1, v2);
        mul36 = _mm256_mul_ps(mul36, v3);
        mul36 = _mm256_mul_ps(mul36, v4);

        v1 = _mm256_add_ps(mul14, mul25);
        v1 = _mm256_add_ps(v1, mul36);

        v2 = _mm256_permute2f128_ps(v1, v1, 0x31);
        v1 = _mm256_sub_ps(v1, v2);

        v2 = _mm256_permute_ps(v1, _MM_SHUFFLE(2, 3, 0, 1));
        v1 = _mm256_add_ps(v1, v2);
        v2 = _mm256_permute_ps(v1, _MM_SHUFFLE(0, 1, 2, 3));
        v1 = _mm256_add_ps(v1, v2);

        return _mm256_cvtss_f32(v1);
    }

    static inline matrix VECTOR_CALL matrix_inverse(matrix m, float det) noexcept {
        assert(0.0f != det);

        auto tmp00 = _mm256_blend_ps(m.v12, m.v34, 0b00001111);

        auto tmp01 = _mm256_permutevar8x32_ps(tmp00,
            _mm256_set_epi32(6, 6, 2, 2, 6, 6, 2, 2));
        auto tmp02 = _mm256_permutevar8x32_ps(tmp00,
            _mm256_set_epi32(7, 7, 3, 3, 7, 7, 3, 3));
        auto tmp03 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(3, 7, 7, 7, 3, 7, 7, 7));
        auto tmp04 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(2, 6, 6, 6, 2, 6, 6, 6));
        auto tmp05 = _mm256_permutevar8x32_ps(m.v12,
            _mm256_set_epi32(2, 2, 2, 6, 2, 2, 2, 6));
        auto tmp06 = _mm256_permutevar8x32_ps(m.v12,
            _mm256_set_epi32(3, 3, 3, 7, 3, 3, 3, 7));

        auto mul0 = _mm256_permutevar8x32_ps(m.v12,
            _mm256_set_epi32(0, 0, 0, 4, 1, 1, 1, 5));
        auto mul1 = _mm256_sub_ps(_mm256_mul_ps(tmp01, tmp03), _mm256_mul_ps(tmp04, tmp02));
        auto res1 = _mm256_mul_ps(mul0, mul1);

        mul0 = _mm256_permutevar8x32_ps(tmp00,
            _mm256_set_epi32(4, 4, 0, 0, 5, 5, 1, 1));
        mul1 = _mm256_sub_ps(_mm256_mul_ps(tmp05, tmp03), _mm256_mul_ps(tmp04, tmp06));
        res1 = _mm256_sub_ps(res1, _mm256_mul_ps(mul0, mul1));

        mul0 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(0, 4, 4, 4, 1, 5, 5, 5));
        mul1 = _mm256_sub_ps(_mm256_mul_ps(tmp05, tmp02), _mm256_mul_ps(tmp01, tmp06));
        res1 = _mm256_add_ps(res1, _mm256_mul_ps(mul0, mul1));

        tmp01 = _mm256_permutevar8x32_ps(tmp00,
            _mm256_set_epi32(5, 5, 1, 1, 5, 5, 1, 1));
        tmp02 = _mm256_permutevar8x32_ps(tmp00,
            _mm256_set_epi32(6, 6, 2, 2, 7, 7, 3, 3));
        tmp03 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(2, 6, 6, 6, 3, 7, 7, 7));
        tmp04 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(1, 5, 5, 5, 1, 5, 5, 5));
        tmp05 = _mm256_permutevar8x32_ps(m.v12,
            _mm256_set_epi32(1, 1, 1, 5, 1, 1, 1, 5));
        tmp06 = _mm256_permutevar8x32_ps(m.v12,
            _mm256_set_epi32(2, 2, 2, 6, 3, 3, 3, 7));

        mul0 = _mm256_permutevar8x32_ps(m.v12,
            _mm256_set_epi32(0, 0, 0, 4, 0, 0, 0, 4));
        mul1 = _mm256_sub_ps(_mm256_mul_ps(tmp01, tmp03), _mm256_mul_ps(tmp04, tmp02));
        auto res2 = _mm256_mul_ps(mul0, mul1);

        mul0 = _mm256_permutevar8x32_ps(tmp00,
            _mm256_set_epi32(4, 4, 0, 0, 4, 4, 0, 0));
        mul1 = _mm256_sub_ps(_mm256_mul_ps(tmp05, tmp03), _mm256_mul_ps(tmp04, tmp06));
        res2 = _mm256_sub_ps(res2, _mm256_mul_ps(mul0, mul1));

        mul0 = _mm256_permutevar8x32_ps(m.v34,
            _mm256_set_epi32(0, 4, 4, 4, 0, 4, 4, 4));
        mul1 = _mm256_sub_ps(_mm256_mul_ps(tmp05, tmp02), _mm256_mul_ps(tmp01, tmp06));
        res2 = _mm256_add_ps(res2, _mm256_mul_ps(mul0, mul1));

        tmp00 = _mm256_set_ps(
             1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,  1.0f);
        res1 = _mm256_mul_ps(res1, tmp00);
        res2 = _mm256_mul_ps(res2, tmp00);

        tmp00 = _mm256_set1_ps(det);
        res1 = _mm256_div_ps(res1, tmp00);
        res2 = _mm256_div_ps(res2, tmp00);

        return {
            .v34 = res2,
            .v12 = res1
        };
    }

    static inline matrix VECTOR_CALL quaternion_to_rotation_matrix(vector q) noexcept {
        // TODO
        return matrix_identity();
    }
}
