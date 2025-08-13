// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

namespace QCEMathDemo {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(MathType) \
        CU_VALUED_ENUM_UNIT(Scalar, 1) \
        CU_VALUED_ENUM_UNIT(Vector, 4) \
        CU_VALUED_ENUM_UNIT(Matrix, 16) \
    CU_END_ENUM(MathType) \
    CU_BEGIN_ENUM(MathOperation) \
        CU_ENUM_UNIT(None) \
        CU_ENUM_UNIT(VectorAddition) \
        CU_ENUM_UNIT(VectorSubtraction) \
        CU_ENUM_UNIT(VectorMultiplication) \
        CU_ENUM_UNIT(VectorDivision) \
        CU_ENUM_UNIT(VectorDotProduct) \
        CU_ENUM_UNIT(VectorCrossProduct) \
        CU_ENUM_UNIT(VectorScalarMultiplication) \
        CU_ENUM_UNIT(VectorScalarDivision) \
        CU_ENUM_UNIT(VectorLength) \
        CU_ENUM_UNIT(VectorNormalization) \
        CU_ENUM_UNIT(VectorMatrixMultiplication) \
        CU_ENUM_UNIT(MatrixAddition) \
        CU_ENUM_UNIT(MatrixSubtraction) \
        CU_ENUM_UNIT(MatrixMultiplication) \
        CU_ENUM_UNIT(MatrixTransposition) \
        CU_ENUM_UNIT(MatrixDeterminant) \
        CU_ENUM_UNIT(MatrixInvert) \
        CU_ENUM_UNIT(MatrixVectorMultiplication) \
    CU_END_ENUM(MathOperation)

#include <cu/enum-utils.hpp>
}
