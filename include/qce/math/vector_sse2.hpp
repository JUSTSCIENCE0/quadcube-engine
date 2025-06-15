// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

// do not include manually
// used only in the composition of the math.hpp

#if defined(VECTOR_IMPLEMENTATION)
#  error "Vector already implemented in other file. Check includes!"
#endif

#define VECTOR_IMPLEMENTATION "sse2"

#if defined(_MSC_VER)
#  define VECTOR_CALL __vectorcall
#else
#  define VECTOR_CALL
#endif

#include <xmmintrin.h> // sse
#include <emmintrin.h> // sse2

#include <cassert>

namespace QCE {
    // vector type

    using vector = __m128;

// GCC & CLang already have operators for __m128
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
        return _mm_set_ps(w, z, y, x);
    }

    static inline vector vector_init(const float* arr) noexcept {
        assert(arr);
        return _mm_loadu_ps(arr);
    }

    static inline vector VECTOR_CALL vector_zero() noexcept {
        return _mm_setzero_ps();
    }

    static inline vector VECTOR_CALL vector_one() noexcept {
        return _mm_set1_ps(1.0);
    }

    // functions
    static inline bool VECTOR_CALL vector_is_equal(
            vector lhs, vector rhs,
            float absolute_epsilon = 1e-5f, float relative_epsilon = 1e-5f) noexcept {
        // calc abs delta
        static const auto sign_mask = _mm_set1_ps(-0.f);
        auto abs_delta = _mm_sub_ps(lhs, rhs);
        abs_delta = _mm_andnot_ps(sign_mask, abs_delta);

        // compare with absolute_epsilon
        auto abs_eps = _mm_set1_ps(absolute_epsilon);
        auto abs_res_vec = _mm_cmplt_ps(abs_delta, abs_eps);
        auto abs_res = _mm_movemask_ps(abs_res_vec);
        if (abs_res == 0b1111)
            return true;

        // compare with relative_epsilon
        auto max_val = _mm_max_ps(lhs, rhs);
        auto rel_eps = _mm_set1_ps(relative_epsilon);
        rel_eps = _mm_mul_ps(max_val, rel_eps);
        auto rel_res_vec = _mm_cmplt_ps(abs_delta, rel_eps);
        auto rel_res = _mm_movemask_ps(rel_res_vec);
        if (rel_res == 0b1111)
            return true;

        return false;
    }

    static inline float VECTOR_CALL vector_length(vector value) noexcept {
        value = _mm_mul_ps(value, value);
        auto shuf = _mm_shuffle_ps(value, value, _MM_SHUFFLE(2, 3, 0, 1));
        auto sums = _mm_add_ps(value, shuf);
        shuf = _mm_movehl_ps(shuf, sums);
        sums = _mm_add_ss(sums, shuf);
        sums = _mm_sqrt_ps(sums);
        return _mm_cvtss_f32(sums);
    }

    static inline vector VECTOR_CALL vector_normalize(vector value) noexcept {
        auto vec_len = vector_length(value);
        return value / vec_len;
    }

    static inline float VECTOR_CALL vector_dot_product(vector lhs, vector rhs) noexcept {
        auto product = _mm_mul_ps(lhs, rhs);
        auto shuf = _mm_shuffle_ps(product, product, _MM_SHUFFLE(2, 3, 0, 1));
        auto sums = _mm_add_ps(product, shuf);
        shuf = _mm_movehl_ps(shuf, sums);
        sums = _mm_add_ss(sums, shuf);
        return _mm_cvtss_f32(sums);
    }

    static inline vector VECTOR_CALL vector_cross_product(vector lhs, vector rhs) noexcept {
        auto lhs_yzx = _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 0, 2, 1));
        auto rhs_yzx = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 0, 2, 1));

        lhs = _mm_mul_ps(lhs, rhs_yzx);
        rhs = _mm_mul_ps(rhs, lhs_yzx);
        auto res = _mm_sub_ps(lhs, rhs);
        return _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 0, 2, 1));
    }

    static inline void VECTOR_CALL vector_copy(vector value, float* dst) noexcept {
        assert(dst);
        _mm_storeu_ps(dst, value);
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
        auto zero_vector = _mm_setzero_ps();
        return _mm_sub_ps(zero_vector, value);
    }

    static inline vector VECTOR_CALL operator+(vector lhs, vector rhs) noexcept {
        return _mm_add_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator+=(vector& lhs, vector rhs) noexcept {
        lhs = lhs + rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator-(vector lhs, vector rhs) noexcept {
        return { _mm_sub_ps(lhs, rhs) };
    }

    static inline vector& VECTOR_CALL operator-=(vector& lhs, vector rhs) noexcept {
        lhs = lhs - rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator*(vector lhs, vector rhs) noexcept {
        return _mm_mul_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator*=(vector& lhs, vector rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator/(vector lhs, vector rhs) noexcept {
        return _mm_div_ps(lhs, rhs);
    }

    static inline vector& VECTOR_CALL operator/=(vector& lhs, vector rhs) noexcept {
        lhs = lhs / rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator*(vector lhs, float rhs) noexcept {
        auto scale = _mm_set1_ps(rhs);
        return _mm_mul_ps(lhs, scale);
    }

    static inline vector& VECTOR_CALL operator*= (vector& lhs, float rhs) noexcept {
        lhs = lhs * rhs;
        return lhs;
    }

    static inline vector VECTOR_CALL operator/(vector lhs, float rhs) noexcept {
        auto scale = _mm_set1_ps(rhs);
        return _mm_div_ps(lhs, scale);
    }

    static inline vector& VECTOR_CALL operator/= (vector& lhs, float rhs) noexcept {
        lhs = lhs / rhs;
        return lhs;
    }
#endif
}
