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

#define VECTOR_IMPLEMENTATION "avx512"

#if defined(_MSC_VER)
#  define VECTOR_CALL __vectorcall
#else
#  define VECTOR_CALL
#endif

#include <immintrin.h> // avx

#include <cassert>

namespace QCE {
    inline constexpr auto REQUIRED_ALIGNAS = 64;

    // vector type
    using vector = __m512;

// GCC & CLang already have operators for __m512
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
        return _mm512_set_ps(
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            w,    z,    y,    x
        );
    }

    static inline vector vector_init(const float* arr) noexcept {
        assert(arr);
        return _mm512_maskz_loadu_ps(0b1111u, arr);
    }

    static inline vector VECTOR_CALL vector_zero() noexcept {
        return _mm512_setzero_ps();
    }

    static inline vector VECTOR_CALL vector_one() noexcept {
        return _mm512_set1_ps(1.0);
    }

    // functions
    static inline bool VECTOR_CALL vector_is_equal(
            vector lhs, vector rhs,
            float absolute_epsilon = 1e-5f, float relative_epsilon = 1e-5f) noexcept {
        // calc abs delta
        static const auto sign_mask = _mm_set1_ps(-0.f);
        auto abs_delta = _mm512_abs_ps(_mm512_sub_ps(lhs, rhs));

        // compare with absolute_epsilon
        auto abs_eps = _mm512_set1_ps(absolute_epsilon);
        auto abs_res = _mm512_cmplt_ps_mask(abs_delta, abs_eps);
        if (abs_res == 0xffffu)
            return true;

        // compare with relative_epsilon
        auto max_val = _mm512_max_ps(lhs, rhs);
        auto rel_eps = _mm512_set1_ps(relative_epsilon);
        rel_eps = _mm512_mul_ps(max_val, rel_eps);
        auto rel_res = _mm512_cmplt_ps_mask(abs_delta, rel_eps);
        if (rel_res == 0xffffu)
            return true;

        return false;
    }

    static inline float VECTOR_CALL vector_length(vector value) noexcept {
        value = _mm512_mul_ps(value, value);
        auto shuf = _mm512_movehdup_ps(value);
        auto sums = _mm512_add_ps(value, shuf);
        shuf = _mm512_unpackhi_ps(sums, sums);
        sums = _mm512_add_ps(sums, shuf);
        sums = _mm512_sqrt_ps(sums);
        return _mm_cvtss_f32(_mm512_castps512_ps128(sums));
    }

    static inline vector VECTOR_CALL vector_normalize(vector value) noexcept {
        auto vec_len = vector_length(value);
        return value / vec_len;
    }

    static inline float VECTOR_CALL vector_dot_product(vector lhs, vector rhs) noexcept {
        auto product = _mm512_mul_ps(lhs, rhs);
        auto shuf = _mm512_movehdup_ps(product);
        auto sums = _mm512_add_ps(product, shuf);
        shuf = _mm512_unpackhi_ps(sums, sums);
        sums = _mm512_add_ps(sums, shuf);
        return _mm_cvtss_f32(_mm512_castps512_ps128(sums));
    }

    static inline vector VECTOR_CALL vector_cross_product(vector lhs, vector rhs) noexcept {
        auto lhs_yzx = _mm512_permute_ps(lhs, _MM_SHUFFLE(3, 0, 2, 1));
        auto rhs_yzx = _mm512_permute_ps(rhs, _MM_SHUFFLE(3, 0, 2, 1));

        lhs = _mm512_mul_ps(lhs, rhs_yzx);
        rhs = _mm512_mul_ps(rhs, lhs_yzx);
        auto res = _mm512_sub_ps(lhs, rhs);
        //auto res = _mm512_fmsub_ps(lhs, rhs_yzx, _mm512_mul_ps(rhs, lhs_yzx));
        return _mm512_permute_ps(res, _MM_SHUFFLE(3, 0, 2, 1));
    }

    static inline void VECTOR_CALL vector_copy(vector value, float* dst) noexcept {
        assert(dst);
        _mm512_mask_storeu_ps(dst, 0b1111u, value);
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
        auto zero_vector = _mm512_setzero_ps();
        return _mm512_sub_ps(zero_vector, value);
    }

    static inline vector VECTOR_CALL operator+(vector lhs, vector rhs) noexcept {
        return _mm512_add_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator+=(vector& lhs, vector rhs) noexcept {
        lhs = lhs + rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator-(vector lhs, vector rhs) noexcept {
        return _mm512_sub_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator-=(vector& lhs, vector rhs) noexcept {
        lhs = lhs - rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator*(vector lhs, vector rhs) noexcept {
        return _mm512_mul_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator*=(vector& lhs, vector rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator/(vector lhs, vector rhs) noexcept {
        return _mm512_div_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator/=(vector& lhs, vector rhs) noexcept {
        lhs = lhs / rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator*(vector lhs, float rhs) noexcept {
        auto scale = _mm512_set1_ps(rhs);
        return _mm512_mul_ps(lhs, scale);
    }

    static inline vector& VECTOR_CALL operator*= (vector& lhs, float rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator/(vector lhs, float rhs) noexcept {
        auto scale = _mm512_set1_ps(rhs);
        return _mm512_div_ps(lhs, scale);
    }

    static inline vector& VECTOR_CALL operator/= (vector& lhs, float rhs) noexcept {
        lhs = lhs / rhs;
        return lhs;
    }
#endif
}
