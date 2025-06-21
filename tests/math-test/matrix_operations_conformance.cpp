// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <cu/test-utils.hpp>

#include "matrix_operations.hpp"

CU_CONFORMANCE_TEST_SIMD(
    MatrixAddition,
    QCE_TEST_DATA_PATH,
    "16000000_float32.bin",
    "vector4_float32_sum.bin",
    QCE::matrix_addition,
    (def)
)
