// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// do not include manually
// used for generation matrix_operations.hpp

#include <stdint.h>

namespace QCE {
    void CU_SIMD_IFACE(matrix_addition)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(matrix_subtraction)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(matrix_multiplication)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(vector_matrix_multiplication)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(matrix_vector_multiplication)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(matrix_calc_transpose)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(matrix_calc_determinant)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(matrix_calc_inverse)
        (const float* values, int64_t count, float* results);
}
