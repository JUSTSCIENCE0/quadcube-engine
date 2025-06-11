// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// do not include manually
// used for generation vector_operations_<compile unut type>.cpp

#pragma once

#include <qce/math.hpp>

#include "vector_operations.hpp"

#include <cassert>

namespace QCE {
#define VECTOR_BINARY_OPERATOR(name, operation) \
    void CU_SIMD(vector_ ##name)(const float* values, int64_t count, float* results) { \
        assert(values && count && results); \
        assert(count % 4 == 0); \
\
        while (count > 0) { \
            auto lhs = vector_init(values[0], values[1], values[2], values[3]); \
            auto rhs = vector_init(values[4], values[5], values[6], values[7]); \
\
            auto res = lhs operation rhs; \
            vector_copy(res, results); \
\
            values += 8; \
            count -= 8; \
            results += 4; \
        } \
    }

    VECTOR_BINARY_OPERATOR(addition,       +)
    VECTOR_BINARY_OPERATOR(subtraction,    -)
    VECTOR_BINARY_OPERATOR(multiplication, *)
    VECTOR_BINARY_OPERATOR(division,       /)

#define VECTOR_WITH_SCALAR_OPERATION(name, operation) \
    void CU_SIMD(vector_scalar_ ##name)(const float* values, int64_t count, float* results) { \
        assert(values && count && results); \
        assert(count % 5 == 0); \
\
        while (count > 0) { \
            auto rhs = values[0]; \
            auto lhs = vector_init(values + 1); \
\
            auto res = lhs operation rhs; \
            vector_copy(res, results); \
\
            values += 5; \
            count -= 5; \
            results += 4; \
        } \
    }

    VECTOR_WITH_SCALAR_OPERATION(multiplication, *)
    VECTOR_WITH_SCALAR_OPERATION(division,       /)
}

