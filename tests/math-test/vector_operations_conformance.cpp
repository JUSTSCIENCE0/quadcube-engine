// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#define USE_TINY_DATASET // TODO: CMake implementation

#ifdef USE_TINY_DATASET
#  define SOURCE_FILE "500000_float32.bin"
#else
#  define SOURCE_FILE "4000000_float32.bin"
#endif

#include <cu/test-utils.hpp>

#include "vector_operations.hpp"

CU_CONFORMANCE_TEST_SIMD(
    VectorAddition,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_sum.bin",
    QCE::vector_addition,
    ( def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorSubtraction,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_diff.bin",
    QCE::vector_subtraction,
    ( def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorMultiplication,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_mul.bin",
    QCE::vector_multiplication,
    ( def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorDivision,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_div.bin",
    QCE::vector_division,
    ( def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorScalarMultiplication,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_scalar_mul.bin",
    QCE::vector_scalar_multiplication,
    (def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorScalarDivision,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_scalar_div.bin",
    QCE::vector_scalar_division,
    (def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorLength,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_length.bin",
    QCE::vector_calc_length,
    (def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorNormalization,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_normalize.bin",
    QCE::vector_calc_normalized,
    (def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorDotProduct,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_dot_product.bin",
    QCE::vector_calc_dot_product,
    (def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorCrossProduct,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_cross_product.bin",
    QCE::vector_calc_cross_product,
    (def, sse2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorCompare,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "vector4_float32_zero.bin",
    QCE::vector_equality,
    (def, sse2, avx512)
)
