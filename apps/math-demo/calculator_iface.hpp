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
        CU_SIMD_ABSTRACT_MTD(std::vector<float>, ProcessImpl, MathOperation operation);

        CU_SIMD_BASE_ATTR(std::vector<float>, m_input_data, {});
    };
#endif // CU_SIMD_CLASS_DECLARATION_SECTION

    CU_SIMD_ADD_FACTORY()
}
