// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <cu/test-utils.hpp>

#include "matrix_operations.hpp"

CU_PERFORMANCE_TEST_SIMD(
    MatrixAddition,
    QCE_TEST_DATA_PATH,
    "4000000_float32.bin",
    QCE::matrix_addition,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    MatrixSubtraction,
    QCE_TEST_DATA_PATH,
    "4000000_float32.bin",
    QCE::matrix_subtraction,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    MatrixMultiplication,
    QCE_TEST_DATA_PATH,
    "4000000_float32.bin",
    QCE::matrix_multiplication,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    VectorMatrixMultiplication,
    QCE_TEST_DATA_PATH,
    "4000000_float32.bin",
    QCE::vector_matrix_multiplication,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    MatrixVectorMultiplication,
    QCE_TEST_DATA_PATH,
    "4000000_float32.bin",
    QCE::matrix_vector_multiplication,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    MatrixTranspose,
    QCE_TEST_DATA_PATH,
    "4000000_float32.bin",
    QCE::matrix_calc_transpose,
    (def, sse2)
)

CU_PERFORMANCE_TEST_SIMD(
    MatrixDeterminant,
    QCE_TEST_DATA_PATH,
    "4000000_float32.bin",
    QCE::matrix_calc_determinant,
    (def)
)

CU_PERFORMANCE_TEST_SIMD(
    MatrixInverse,
    QCE_TEST_DATA_PATH,
    "4000000_float32.bin",
    QCE::matrix_calc_inverse,
    (def)
)
