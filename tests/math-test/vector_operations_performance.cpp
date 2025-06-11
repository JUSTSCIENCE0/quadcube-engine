// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#define CU_PRINT_PERFORMANCE_TEST_RESULT

#include <cu/test-utils.hpp>

#include "vector_operations.hpp"

CU_PERFORMANCE_TEST_SIMD(
    VectorAddition,
    QCE_TEST_DATA_PATH,
    "16000000_float32.bin",
    QCE::vector_addition,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorSubtraction,
    QCE_TEST_DATA_PATH,
    "16000000_float32.bin",
    QCE::vector_subtraction,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorMultiplication,
    QCE_TEST_DATA_PATH,
    "16000000_float32.bin",
    QCE::vector_multiplication,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorDivision,
    QCE_TEST_DATA_PATH,
    "16000000_float32.bin",
    QCE::vector_division,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorScalarMultiplication,
    QCE_TEST_DATA_PATH,
    "16000000_float32.bin",
    QCE::vector_scalar_multiplication,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorScalarDivision,
    QCE_TEST_DATA_PATH,
    "16000000_float32.bin",
    QCE::vector_scalar_division,
    (def, sse2)
)
