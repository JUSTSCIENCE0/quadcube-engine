// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// do not include manually
// used for generation calculator_<compile unut type>.hpp

#pragma once

#include "calculator.hpp"

#include <qce/math.hpp>

#include <iostream>

namespace QCEMathDemo {
#ifdef CU_SIMD_BASE_IMPL
    CU_SIMD_CLASS_IMPL::CU_SIMD_CLASS_IMPL(std::vector<float> input_data) :
        m_input_data(std::move(input_data)) {}

    std::vector<float> CU_SIMD_CLASS_IMPL::Process(MathOperation operation) {
        switch (operation) {
        case MathOperation::VectorAddition:
            return VectorAddition();
        // TODO: other
        default:
            std::cout << "Error: selected unsupported operation" << std::endl;
            return {};
        }
    }
#endif

#ifdef CU_SIMD_DERIVED_IMPL
    CU_SIMD_CLASS_IMPL::CU_SIMD_CLASS_IMPL(std::vector<float> input_data) :
        Calculator(std::move(input_data)) {}

    std::string CU_SIMD_CLASS_IMPL::Description() {
        std::string result = CU_EXPAND_STR(CU_SIMD_CLASS_IMPL);
        result += " Math implementation: vector - ";
        result += VECTOR_IMPLEMENTATION;
        result += "; matrix - ";
        result += MATRIX_IMPLEMENTATION;

        return result;
    }

    std::vector<float> CU_SIMD_CLASS_IMPL::VectorAddition() {
        using namespace QCE;

        auto count = m_input_data.size();
        assert(count % 8 == 0);

        std::vector<float> result(count / 2);
        auto values  = m_input_data.data();
        auto results = result.data();

        while (count > 0) {
            auto lhs = vector_init(values[0], values[1], values[2], values[3]);
            auto rhs = vector_init(values[4], values[5], values[6], values[7]);

            auto res = lhs + rhs;
            vector_copy(res, results);

            values += 8;
            count -= 8;
            results += 4;
        }

        return result;
    }
#endif
}
