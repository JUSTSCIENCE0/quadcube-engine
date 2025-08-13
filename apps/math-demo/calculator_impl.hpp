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
        return ProcessImpl(operation);
    }
#endif

#ifdef CU_SIMD_DERIVED_IMPL
    template<
        MathType lhs_type, MathType rhs_type, MathType res_type,
        auto lhs_init, auto rhs_init, auto operation, auto res_store
    >
    std::vector<float> CalculateDyadic(const std::vector<float>& input_data) {
        constexpr size_t iteration_read_size = lhs_type + rhs_type;
        constexpr size_t iteration_write_size = res_type;

        const float* values = input_data.data();
        size_t count = input_data.size();
        assert(count % iteration_read_size == 0);

        std::vector<float> result(count * iteration_write_size / iteration_read_size);
        float* results = result.data();

        //std::cout << "Debug Info:" << std::endl;
        //std::cout << "\t" << std::endl;

        while (count > 0) {
            auto lhs = lhs_init(values);
            auto rhs = rhs_init(values + lhs_type);

            auto res = operation(lhs, rhs);
            res_store(res, results);

            values += iteration_read_size;
            count -= iteration_read_size;
            results += iteration_write_size;
        }

        return result;
    }

    CU_SIMD_CLASS_IMPL::CU_SIMD_CLASS_IMPL(std::vector<float> input_data) :
        Calculator(std::move(input_data)) {}

    std::vector<float> CU_SIMD_CLASS_IMPL::ProcessImpl(MathOperation operation) {
        using namespace QCE;

        static constexpr auto vector_init_impl = static_cast<vector(*)(const float*)>(vector_init);

        switch (operation) {
        case MathOperation::VectorAddition:
            return {};
        case MathOperation::VectorCrossProduct:
            return CalculateDyadic<
                MathType::Vector, MathType::Vector, MathType::Vector,
                vector_init_impl, vector_init_impl, vector_cross_product, vector_copy>(m_input_data);
            // TODO: other
        default:
            std::cout << "Error: selected unsupported operation" << std::endl;
            return {};
        }
    }

    std::string CU_SIMD_CLASS_IMPL::Description() {
        std::string result = CU_EXPAND_STR(CU_SIMD_CLASS_IMPL);
        result += " Math implementation: vector - ";
        result += VECTOR_IMPLEMENTATION;
        result += "; matrix - ";
        result += MATRIX_IMPLEMENTATION;

        return result;
    }

#endif
}
