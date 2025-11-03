// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifdef USE_TINY_DATASET
#  define SOURCE_FILE "500000_float32.bin"
#else
#  define SOURCE_FILE "4000000_float32.bin"
#endif

#include <cu/test-utils.hpp>

#include "matrix_operations.hpp"

CU_CONFORMANCE_TEST_SIMD(
    MatrixAddition,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_sum.bin",
    QCE::matrix_addition,
    (def, sse2, avx2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    MatrixSubtraction,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_diff.bin",
    QCE::matrix_subtraction,
    (def, sse2, avx2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    MatrixMultiplication,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_mul.bin",
    QCE::matrix_multiplication,
    (def, sse2, avx2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    VectorMatrixMultiplication,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_vector_matrix_mul.bin",
    QCE::vector_matrix_multiplication,
    (def, sse2, avx2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    MatrixVectorMultiplication,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_matrix_vector_mul.bin",
    QCE::matrix_vector_multiplication,
    (def, sse2, avx2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    MatrixTranspose,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_transpose.bin",
    QCE::matrix_calc_transpose,
    (def, sse2, avx2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    MatrixFromQuaternion,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_quaternion_to_rotation.bin",
    QCE::matrix_from_quaternion,
    (def, sse2, avx2) /*TODO avx512*/
)

CU_CONFORMANCE_TEST_SIMD(
    QuaternionFromMatrix,
    QCE_TEST_DATA_PATH,
    "matrix4x4_float32_quaternion_to_rotation.bin",
    "quaternions.bin",
    QCE::matrix_to_quaternion,
    (def, sse2, avx2)  /*TODO avx512*/
)

CU_CONFORMANCE_TEST_SIMD(
    CameraToLhView,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_lh_view_matrices.bin",
    QCE::camera_to_lh_view,
    (def, sse2, avx2) /*TODO avx512*/
)

#if !defined(CU_COMPILER_GCC)
CU_CONFORMANCE_TEST_SIMD(
    MatrixDeterminant,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_det.bin",
    QCE::matrix_calc_determinant,
    (def, sse2, avx2, avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    MatrixInverse,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_invert.bin",
    QCE::matrix_calc_inverse,
    (def, sse2, avx2, avx512)
)
#else
// GCC optimization breaks conformance tests on avx512
// GCC 13.3.0 x86_64-linux-gnu
CU_CONFORMANCE_TEST_SIMD(
    MatrixDeterminant,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_det.bin",
    QCE::matrix_calc_determinant,
    (def, sse2)
)

CU_CONFORMANCE_TEST_SIMD_WEAK(
    MatrixDeterminantWeakAVX512,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_det.bin",
    QCE::matrix_calc_determinant,
    (avx512)
)

CU_CONFORMANCE_TEST_SIMD(
    MatrixInverse,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_invert.bin",
    QCE::matrix_calc_inverse,
    (def, sse2)
)

CU_CONFORMANCE_TEST_SIMD_WEAK(
    MatrixInverseWeakAVX512,
    QCE_TEST_DATA_PATH,
    SOURCE_FILE,
    "matrix4x4_float32_invert.bin",
    QCE::matrix_calc_inverse,
    (avx512)
)
#endif

