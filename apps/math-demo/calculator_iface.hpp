// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// do not include manually
// used for generation calculator.hpp

#include "operations.hpp"

#include <vector>
#include <string>

namespace QCEMathDemo {
#ifdef CU_SIMD_CLASS_DECLARATION_SECTION
    class CU_SIMD_CLASS {
    public:
        // ctor/dtor
        CU_SIMD_CTOR(std::vector<float> input_data);
        CU_SIMD_DTOR = default;

        CU_SIMD_BASE_MTD(std::vector<float>, Process, MathOperation operation);

        CU_SIMD_ABSTRACT_MTD(std::string, Description);

    protected:
        CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorAddition);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorSubtraction);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorMultiplication);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorDivision);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorScalarMultiplication);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorScalarDivision);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorLength);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorNormalization);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorDotProduct);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorCrossProduct);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, VectorMatrixMultiplication);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, MatrixAddition);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, MatrixSubtraction);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, MatrixMultiplication);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, MatrixTransposition);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, MatrixDeterminant);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, MatrixInvert);
        //CU_SIMD_ABSTRACT_MTD(std::vector<float>, MatrixVectorMultiplication);

        CU_SIMD_BASE_ATTR(std::vector<float>, m_input_data, {});
    };
#endif // CU_SIMD_CLASS_DECLARATION_SECTION

    CU_SIMD_ADD_FACTORY()
}
