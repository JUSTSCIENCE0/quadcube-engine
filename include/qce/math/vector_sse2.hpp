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

    // initializers
    static inline vector VECTOR_CALL vector_init(
            float x, float y, float z, float w) noexcept {
        return _mm_set_ps(x, y, z, w);
    }

    static inline vector VECTOR_CALL vector_zero() noexcept {
        return _mm_setzero_ps();
    }

    static inline vector VECTOR_CALL vector_one() noexcept {
        return _mm_set1_ps(1.0);
    }

    // functions
    static inline void VECTOR_CALL vector_copy(vector value, float* dst) noexcept {
        assert(dst);
        value = _mm_shuffle_ps(value, value, _MM_SHUFFLE(0, 1, 2, 3));
        _mm_storeu_ps(dst, value);
    }

    // member access
    // TODO - getters

    // operators
    static inline vector VECTOR_CALL operator+ (vector value) noexcept {
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
        return _mm_sub_ps(lhs, rhs);
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

}
