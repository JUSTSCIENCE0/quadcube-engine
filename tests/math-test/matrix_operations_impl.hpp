// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// do not include manually
// used for generation matrix_operations_<compile unut type>.cpp

#pragma once

#include <qce/math.hpp>

#include "matrix_operations.hpp"

#include <cassert>

namespace QCE {
    void CU_SIMD(matrix_addition)(const float* values, int64_t count, float* results) {
        assert(values && count && results);
        assert(count % 32 == 0);
        while (count > 0) {
            auto lhs = matrix_init(values);
            auto rhs = matrix_init(values + 16);
            auto res = lhs + rhs;
            matrix_copy(res, results);

            values += 32;
            count -= 32;
            results += 16;
        }
    }

    void CU_SIMD(matrix_subtraction)(const float* values, int64_t count, float* results) {
        assert(values && count && results);
        assert(count % 32 == 0);
        while (count > 0) {
            auto lhs = matrix_init(values);
            auto rhs = matrix_init(values + 16);
            auto res = lhs - rhs;
            matrix_copy(res, results);

            values += 32;
            count -= 32;
            results += 16;
        }
    }

    void CU_SIMD(matrix_multiplication)(const float* values, int64_t count, float* results) {
        assert(values && count && results);
        assert(count % 32 == 0);
        while (count > 0) {
            auto lhs = matrix_init(values);
            auto rhs = matrix_init(values + 16);
            auto res = lhs * rhs;
            matrix_copy(res, results);

            values += 32;
            count -= 32;
            results += 16;
        }
    }

    void CU_SIMD(vector_matrix_multiplication)(const float* values, int64_t count, float* results) {
        assert(values && count && results);
        assert(count % 20 == 0);
        while (count > 0) {
            auto lhs = vector_init(values);
            auto rhs = matrix_init(values + 4);
            auto res = lhs * rhs;
            vector_copy(res, results);

            values += 20;
            count -= 20;
            results += 4;
        }
    }

    void CU_SIMD(matrix_vector_multiplication)(const float* values, int64_t count, float* results) {
        assert(values && count && results);
        assert(count % 20 == 0);
        while (count > 0) {
            auto rhs = vector_init(values);
            auto lhs = matrix_init(values + 4);
            auto res = lhs * rhs;
            vector_copy(res, results);

            values += 20;
            count -= 20;
            results += 4;
        }
    }
}

