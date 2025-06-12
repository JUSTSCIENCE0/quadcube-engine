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

#define VECTOR_IMPLEMENTATION "default"

#include <cassert>
#include <cmath>

namespace QCE {

    // vector type
    struct vector final
    {
        float x;
        float y;
        float z;
        float w;
    };

    // initializers
    static inline vector vector_init(
            float x, float y, float z, float w) noexcept {
        return { x, y, z, w };
    }

    static inline vector vector_init(const float* arr) noexcept {
        assert(arr);
        return { arr[0], arr[1], arr[2], arr[3]};
    }

    static inline vector vector_zero() noexcept {
        return { 0.0, 0.0, 0.0, 0.0 };
    }

    static inline vector vector_one() noexcept {
        return { 1.0, 1.0, 1.0, 1.0 };
    }

    // functions
    static inline float vector_length(const vector& value) noexcept {
        auto sqr_x = value.x * value.x;
        auto sqr_y = value.y * value.y;
        auto sqr_z = value.z * value.z;
        auto sqr_w = value.w * value.w;
        return std::sqrtf(sqr_x + sqr_y + sqr_z + sqr_w);
    }

    static inline void vector_copy(const vector& value, float* dst) noexcept {
        assert(dst);
        dst[0] = value.x;
        dst[1] = value.y;
        dst[2] = value.z;
        dst[3] = value.w;
    }

    // member access
    // TODO - getters

    // operators
    static inline vector operator+ (const vector& value) noexcept {
        return value;
    }

    static inline vector operator- (const vector& value) noexcept {
        return { -value.x, -value.y, -value.z, -value.w };
    }

#ifdef __INTELLISENSE__
    // just for disable IntelliSense errors
    static inline vector& operator+=(vector & lhs, const vector & rhs) noexcept;
    static inline vector  operator+(const vector& lhs, const vector& rhs) noexcept;
    static inline vector& operator-=(vector& lhs, const vector& rhs) noexcept;
    static inline vector  operator-(const vector& lhs, const vector& rhs) noexcept;
    static inline vector& operator*=(vector& lhs, const vector& rhs) noexcept;
    static inline vector  operator*(const vector& lhs, const vector& rhs) noexcept;
    static inline vector& operator/=(vector& lhs, const vector& rhs) noexcept;
    static inline vector  operator/(const vector& lhs, const vector& rhs) noexcept;

    static inline vector& operator*= (vector& lhs, float rhs) noexcept;
    static inline vector  operator*(const vector& lhs, float rhs) noexcept;
    static inline vector& operator/= (vector& lhs, float rhs) noexcept;
    static inline vector  operator/(const vector& lhs, float rhs) noexcept;
#endif

#define VECTOR_OPERATION(operation) \
static inline vector& operator operation ##= (vector& lhs, const vector& rhs) noexcept { \
    lhs.x operation ##= rhs.x; \
    lhs.y operation ##= rhs.y; \
    lhs.z operation ##= rhs.z; \
    lhs.w operation ##= rhs.w; \
    return lhs; \
} \
static inline vector operator operation (const vector& lhs, const vector& rhs) noexcept { \
    vector result = { \
        lhs.x operation rhs.x, \
        lhs.y operation rhs.y, \
        lhs.z operation rhs.z, \
        lhs.w operation rhs.w \
    }; \
    return result; \
}

    VECTOR_OPERATION(+)
    VECTOR_OPERATION(-)
    VECTOR_OPERATION(*)
    VECTOR_OPERATION(/)

#undef VECTOR_OPERATION

#define VECTOR_WITH_SCALAR_OPERATION(operation) \
static inline vector& operator operation ##= (vector& lhs, float rhs) noexcept { \
    lhs.x operation ##= rhs; \
    lhs.y operation ##= rhs; \
    lhs.z operation ##= rhs; \
    lhs.w operation ##= rhs; \
    return lhs; \
} \
static inline vector operator operation (const vector& lhs, float rhs) noexcept { \
    vector result = { \
        lhs.x operation rhs, \
        lhs.y operation rhs, \
        lhs.z operation rhs, \
        lhs.w operation rhs \
    }; \
    return result; \
}

    VECTOR_WITH_SCALAR_OPERATION(*)
    VECTOR_WITH_SCALAR_OPERATION(/)

#undef VECTOR_WITH_SCALAR_OPERATION

}
