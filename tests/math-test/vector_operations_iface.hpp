// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// do not include manually
// used for generation vector_operations.hpp

#include <stdint.h>

namespace QCE {
    void CU_SIMD_IFACE(vector_addition)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(vector_subtraction)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(vector_multiplication)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(vector_division)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(vector_scalar_multiplication)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(vector_scalar_division)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(vector_calc_length)
        (const float* values, int64_t count, float* results);
    void CU_SIMD_IFACE(vector_calc_normalized)
        (const float* values, int64_t count, float* results);
}
