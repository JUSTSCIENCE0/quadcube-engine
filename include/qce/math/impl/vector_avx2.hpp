// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

// do not include manually
// used only in the composition of the vector.hpp and matrix implementation

#if defined(VECTOR_IMPLEMENTATION)
#  error "Vector already implemented in other file. Check includes!"
#endif

#define VECTOR_IMPLEMENTATION "avx2"

#if defined(_MSC_VER)
#  define VECTOR_CALL __vectorcall
#else
#  define VECTOR_CALL
#endif

#include <immintrin.h> // avx

#include <cassert>

namespace QCE {
    inline constexpr auto REQUIRED_ALIGNAS = 32;

    // vector type
    using vector = __m256;

// GCC & CLang already have operators for __m256
#ifdef _MSC_VER
    static inline vector& VECTOR_CALL operator+=(vector& lhs, vector rhs) noexcept;
    static inline vector  VECTOR_CALL operator+ (vector  lhs, vector rhs) noexcept;
    static inline vector& VECTOR_CALL operator-=(vector& lhs, vector rhs) noexcept;
    static inline vector  VECTOR_CALL operator- (vector  lhs, vector rhs) noexcept;
    static inline vector& VECTOR_CALL operator*=(vector& lhs, vector rhs) noexcept;
    static inline vector  VECTOR_CALL operator* (vector  lhs, vector rhs) noexcept;
    static inline vector& VECTOR_CALL operator/=(vector& lhs, vector rhs) noexcept;
    static inline vector  VECTOR_CALL operator/ (vector  lhs, vector rhs) noexcept;

    static inline vector& VECTOR_CALL operator*=(vector& lhs, float rhs) noexcept;
    static inline vector  VECTOR_CALL operator* (vector  lhs, float rhs) noexcept;
    static inline vector& VECTOR_CALL operator/=(vector& lhs, float rhs) noexcept;
    static inline vector  VECTOR_CALL operator/ (vector  lhs, float rhs) noexcept;
#endif

    // initializers
    static inline vector VECTOR_CALL vector_init(
            float x, float y, float z, float w) noexcept {
        return _mm256_set_ps(
            0.0f, 0.0f, 0.0f, 0.0f,
            w,    z,    y,    x
        );
    }

    static inline vector vector_init(const float* arr) noexcept {
        assert(arr);
        return _mm256_maskload_ps(arr,
            _mm256_set_epi32(0, 0, 0, 0, -1, -1, -1, -1));
    }

    static inline vector VECTOR_CALL vector_zero() noexcept {
        return _mm256_setzero_ps();
    }

    static inline vector VECTOR_CALL vector_one() noexcept {
        return _mm256_set1_ps(1.0);
    }

    // functions
    static inline bool VECTOR_CALL vector_is_equal(
            vector lhs, vector rhs,
            float absolute_epsilon = 1e-5f, float relative_epsilon = 1e-5f) noexcept {
        // calc abs delta
        static const auto sign_mask = _mm256_set1_ps(-0.f);
        auto abs_delta = _mm256_andnot_ps(sign_mask, _mm256_sub_ps(lhs, rhs));

        // compare with absolute_epsilon
        auto abs_eps = _mm256_set1_ps(absolute_epsilon);
        auto abs_res = _mm256_movemask_ps(_mm256_cmp_ps(abs_delta, abs_eps, _CMP_LT_OQ));
        if (abs_res == 0b11111111)
            return true;

        // compare with relative_epsilon
        auto max_val = _mm256_max_ps(lhs, rhs);
        auto rel_eps = _mm256_set1_ps(relative_epsilon);
        rel_eps = _mm256_mul_ps(max_val, rel_eps);
        auto rel_res = _mm256_movemask_ps(_mm256_cmp_ps(abs_delta, rel_eps, _CMP_LT_OQ));
        if (rel_res == 0b11111111)
            return true;

        return false;
    }

    static inline float VECTOR_CALL vector_length(vector value) noexcept {
        value = _mm256_mul_ps(value, value);
        auto shuf = _mm256_movehdup_ps(value);
        auto sums = _mm256_add_ps(value, shuf);
        shuf = _mm256_unpackhi_ps(sums, sums);
        sums = _mm256_add_ps(sums, shuf);
        sums = _mm256_sqrt_ps(sums);
        return _mm256_cvtss_f32(sums);
    }

    static inline vector VECTOR_CALL vector_normalize(vector value) noexcept {
        auto vec_len = vector_length(value);
        return value / vec_len;
    }

    static inline float VECTOR_CALL vector_dot_product(vector lhs, vector rhs) noexcept {
        auto product = _mm256_mul_ps(lhs, rhs);
        auto shuf = _mm256_movehdup_ps(product);
        auto sums = _mm256_add_ps(product, shuf);
        shuf = _mm256_unpackhi_ps(sums, sums);
        sums = _mm256_add_ps(sums, shuf);
        return _mm256_cvtss_f32(sums);
    }

    static inline vector VECTOR_CALL vector_cross_product(vector lhs, vector rhs) noexcept {
        auto lhs_yzx = _mm256_permute_ps(lhs, _MM_SHUFFLE(3, 0, 2, 1));
        auto rhs_yzx = _mm256_permute_ps(rhs, _MM_SHUFFLE(3, 0, 2, 1));

        lhs = _mm256_mul_ps(lhs, rhs_yzx);
        rhs = _mm256_mul_ps(rhs, lhs_yzx);
        auto res = _mm256_sub_ps(lhs, rhs);
        return _mm256_permute_ps(res, _MM_SHUFFLE(3, 0, 2, 1));
    }

    static inline void VECTOR_CALL vector_copy(vector value, float* dst) noexcept {
        assert(dst);
        _mm256_maskstore_ps(dst, _mm256_set_epi32(0, 0, 0, 0, -1, -1, -1, -1), value);
    }

    // member access
    // TODO - getters

// GCC & CLang already have operators for __m128
#ifdef _MSC_VER
    // operators
    static inline vector VECTOR_CALL operator+ (const vector& value) noexcept {
        return value;
    }

    static inline vector VECTOR_CALL operator- (vector value) noexcept {
        auto zero_vector = _mm256_setzero_ps();
        return _mm256_sub_ps(zero_vector, value);
    }

    static inline vector VECTOR_CALL operator+(vector lhs, vector rhs) noexcept {
        return _mm256_add_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator+=(vector& lhs, vector rhs) noexcept {
        lhs = lhs + rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator-(vector lhs, vector rhs) noexcept {
        return _mm256_sub_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator-=(vector& lhs, vector rhs) noexcept {
        lhs = lhs - rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator*(vector lhs, vector rhs) noexcept {
        return _mm256_mul_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator*=(vector& lhs, vector rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator/(vector lhs, vector rhs) noexcept {
        return _mm256_div_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator/=(vector& lhs, vector rhs) noexcept {
        lhs = lhs / rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator*(vector lhs, float rhs) noexcept {
        auto scale = _mm256_set1_ps(rhs);
        return _mm256_mul_ps(lhs, scale);
    }

    static inline vector& VECTOR_CALL operator*= (vector& lhs, float rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator/(vector lhs, float rhs) noexcept {
        auto scale = _mm256_set1_ps(rhs);
        return _mm256_div_ps(lhs, scale);
    }

    static inline vector& VECTOR_CALL operator/= (vector& lhs, float rhs) noexcept {
        lhs = lhs / rhs;
        return lhs;
    }
#endif
}
