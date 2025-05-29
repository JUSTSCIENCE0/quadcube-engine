// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <cu/test-utils.hpp>

#include "vector_operations.hpp"


TEST(VectorOperationsConformance, Addition) {
    static const std::string INPUT_FILE = QCE_TEST_DATA_PATH "/16000000_float32.bin";
    static const std::string OUTPUT_FILE = QCE_TEST_DATA_PATH "/vector4_float32_sum.bin";

    auto test_list = CU::make_conformance_list({ QCE::vector_addition_def });
    CU::run_conformance_test<float>(INPUT_FILE, OUTPUT_FILE, test_list);
}

TEST(VectorOperationsConformance, Subtraction) {
    static constexpr auto INPUT_FILE = QCE_TEST_DATA_PATH "/16000000_float32.bin";
    static constexpr auto OUTPUT_FILE = QCE_TEST_DATA_PATH "/vector4_float32_diff.bin";

    auto test_list = CU::make_conformance_list({ QCE::vector_subtraction_def });
    CU::run_conformance_test<float>(INPUT_FILE, OUTPUT_FILE, test_list);
}

TEST(VectorOperationsConformance, Multiplication) {
    static constexpr auto INPUT_FILE = QCE_TEST_DATA_PATH "/16000000_float32.bin";
    static constexpr auto OUTPUT_FILE = QCE_TEST_DATA_PATH "/vector4_float32_mul.bin";

    auto test_list = CU::make_conformance_list({ QCE::vector_multiplication_def });
    CU::run_conformance_test<float>(INPUT_FILE, OUTPUT_FILE, test_list);
}

TEST(VectorOperationsConformance, Division) {
    static constexpr auto INPUT_FILE = QCE_TEST_DATA_PATH "/16000000_float32.bin";
    static constexpr auto OUTPUT_FILE = QCE_TEST_DATA_PATH "/vector4_float32_div.bin";

    auto test_list = CU::make_conformance_list({ QCE::vector_division_def });
    CU::run_conformance_test<float>(INPUT_FILE, OUTPUT_FILE, test_list);
}

