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
    using namespace QCE;

    static inline void empty_load(const float*) {}
    static inline float float_load(const float* src) {
        return src[0];
    }
    static inline void float_store(float value, float* dst) {
        dst[0] = value;
    }
    static inline vector vector_load(const float* src) {
        return vector_init(src);
    }
    static inline vector vector_sum(vector lhs, vector rhs) {
        return lhs + rhs;
    }
    static inline vector vector_diff(vector lhs, vector rhs) {
        return lhs - rhs;
    }
    static inline vector vector_mul(vector lhs, vector rhs) {
        return lhs * rhs;
    }
    static inline vector vector_div(vector lhs, vector rhs) {
        return lhs / rhs;
    }
    static inline vector vector_scalar_mul(vector lhs, float rhs) {
        return lhs * rhs;
    }
    static inline vector vector_scalar_div(vector lhs, float rhs) {
        return lhs / rhs;
    }
    static inline matrix matrix_load(const float* src) {
        return matrix_init(src);
    }
    static inline matrix matrix_sum(matrix lhs, matrix rhs) {
        return lhs + rhs;
    }
    static inline matrix matrix_diff(matrix lhs, matrix rhs) {
        return lhs - rhs;
    }
    static inline matrix matrix_inverse_single(matrix val) {
        auto det = matrix_determinant(val);
        return matrix_inverse(val, det);
    }

    template<
        MathType lhs_type, MathType rhs_type, MathType res_type,
        auto lhs_init, auto rhs_init, auto operation, auto res_store
    >
    std::vector<float> CalculateImpl(const std::vector<float>& input_data) {
        static_assert(MathType::Empty != lhs_type);

        constexpr size_t iteration_read_size = lhs_type + rhs_type;
        constexpr size_t iteration_write_size = res_type;

        const float* values = input_data.data();
        size_t count = input_data.size();
        size_t remainder = count % iteration_read_size;
        if (remainder) {
            std::cout << "Warning: input size is not a multiple of a single operation - " <<
                remainder << " values will be skipped" << std::endl;
            count -= remainder;
        }

        std::vector<float> result(count * iteration_write_size / iteration_read_size);
        float* results = result.data();

        //std::cout << "Debug Info:" << std::endl;
        //std::cout << "\t" << std::endl;

        while (count > 0) {
            auto lhs = lhs_init(values);

            if constexpr (MathType::Empty != rhs_type) {
                auto rhs = rhs_init(values + lhs_type);
                auto res = operation(lhs, rhs);
                res_store(res, results);
            }
            else {
                auto res = operation(lhs);
                res_store(res, results);
            }

            values += iteration_read_size;
            count -= iteration_read_size;
            results += iteration_write_size;
        }

        return result;
    }

    CU_SIMD_CLASS_IMPL::CU_SIMD_CLASS_IMPL(std::vector<float> input_data) :
        Calculator(std::move(input_data)) {}

    std::vector<float> CU_SIMD_CLASS_IMPL::ProcessImpl(MathOperation operation) {
        switch (operation) {
        case MathOperation::VectorAddition:
            return CalculateImpl<
                MathType::Vector, MathType::Vector, MathType::Vector,
                vector_load, vector_load, vector_sum, vector_copy>(m_input_data);
        case MathOperation::VectorSubtraction:
            return CalculateImpl<
                MathType::Vector, MathType::Vector, MathType::Vector,
                vector_load, vector_load, vector_diff, vector_copy>(m_input_data);
        case MathOperation::VectorMultiplication:
            return CalculateImpl<
                MathType::Vector, MathType::Vector, MathType::Vector,
                vector_load, vector_load, vector_mul, vector_copy>(m_input_data);
        case MathOperation::VectorDivision:
            return CalculateImpl<
                MathType::Vector, MathType::Vector, MathType::Vector,
                vector_load, vector_load, vector_div, vector_copy>(m_input_data);
        case MathOperation::VectorDotProduct:
            return CalculateImpl<
                MathType::Vector, MathType::Vector, MathType::Scalar,
                vector_load, vector_load, vector_dot_product, float_store>(m_input_data);
        case MathOperation::VectorCrossProduct:
            return CalculateImpl<
                MathType::Vector, MathType::Vector, MathType::Vector,
                vector_load, vector_load, vector_cross_product, vector_copy>(m_input_data);
        case MathOperation::VectorScalarMultiplication:
            return CalculateImpl<
                MathType::Vector, MathType::Scalar, MathType::Vector,
                vector_load, float_load, vector_scalar_mul, vector_copy>(m_input_data);
        case MathOperation::VectorScalarDivision:
            return CalculateImpl<
                MathType::Vector, MathType::Scalar, MathType::Vector,
                vector_load, float_load, vector_scalar_div, vector_copy>(m_input_data);
        case MathOperation::VectorLength:
            return CalculateImpl<
                MathType::Vector, MathType::Empty, MathType::Scalar,
                vector_load, empty_load, vector_length, float_store>(m_input_data);
        case MathOperation::VectorNormalization:
            return CalculateImpl<
                MathType::Vector, MathType::Empty, MathType::Vector,
                vector_load, empty_load, vector_normalize, vector_copy>(m_input_data);
        case MathOperation::VectorMatrixMultiplication:
            return CalculateImpl<
                MathType::Vector, MathType::Matrix, MathType::Vector,
                vector_load, matrix_load, vector_matrix_mul, vector_copy>(m_input_data);
        case MathOperation::MatrixAddition:
            return CalculateImpl<
                MathType::Matrix, MathType::Matrix, MathType::Matrix,
                matrix_load, matrix_load, matrix_sum, matrix_copy>(m_input_data);
        case MathOperation::MatrixSubtraction:
            return CalculateImpl<
                MathType::Matrix, MathType::Matrix, MathType::Matrix,
                matrix_load, matrix_load, matrix_diff, matrix_copy>(m_input_data);
        case MathOperation::MatrixMultiplication:
            return CalculateImpl<
                MathType::Matrix, MathType::Matrix, MathType::Matrix,
                matrix_load, matrix_load, matrix_mul, matrix_copy>(m_input_data);
        case MathOperation::MatrixTransposition:
            return CalculateImpl<
                MathType::Matrix, MathType::Empty, MathType::Matrix,
                matrix_load, empty_load, matrix_transpose, matrix_copy>(m_input_data);
        case MathOperation::MatrixDeterminant:
            return CalculateImpl<
                MathType::Matrix, MathType::Empty, MathType::Scalar,
                matrix_load, empty_load, matrix_determinant, float_store>(m_input_data);
        case MathOperation::MatrixInvert:
            return CalculateImpl<
                MathType::Matrix, MathType::Empty, MathType::Matrix,
                matrix_load, empty_load, matrix_inverse_single, matrix_copy>(m_input_data);
        case MathOperation::MatrixVectorMultiplication:
            return CalculateImpl<
                MathType::Matrix, MathType::Vector, MathType::Vector,
                matrix_load, vector_load, matrix_vector_mul, vector_copy>(m_input_data);
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
