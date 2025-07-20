// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#define CU_PRINT_PERFORMANCE_TEST_RESULT

#ifdef USE_TINY_DATASET
#  define SOURCE_FILE "500000_float32.bin"
#else
#  define SOURCE_FILE "4000000_float32.bin"
#endif

#include <cu/test-utils.hpp>

#include "vector_operations.hpp"

CU_PERFORMANCE_TEST_SIMD(
    VectorAddition,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_addition,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorSubtraction,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_subtraction,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorMultiplication,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_multiplication,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorDivision,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_division,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorScalarMultiplication,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_scalar_multiplication,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorScalarDivision,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_scalar_division,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorLength,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_calc_length,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorNormalization,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_calc_normalized,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorDotProduct,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_calc_dot_product,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorCrossProduct,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_calc_cross_product,
    (def, sse2/*, avx2, avx512*/)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorCompare,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    QCE::vector_equality,
    (def, sse2/*, avx2, avx512*/)
)
