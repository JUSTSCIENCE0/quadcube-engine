// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

// TODO: common-utils: test_utils::conformance_test

#include <gtest/gtest.h>

#include <cu/file-utils.hpp>

#include "vector_operations.hpp"

TEST(VectorOperationsConformance, Addition) {
    static constexpr auto INPUT_FILE = QCE_TEST_DATA_PATH "/16000000_float32.bin";
    static constexpr auto OUTPUT_FILE = QCE_TEST_DATA_PATH "/vector4_float32_sum.bin";

    auto input_data = CU::load_data_from_file<float>(INPUT_FILE);
    ASSERT_FALSE(input_data.empty());

    auto output_data = CU::load_data_from_file<float>(OUTPUT_FILE);
    ASSERT_FALSE(output_data.empty());

    std::vector<float> result_data(output_data.size());
    QCE::vector_addition_def(input_data.data(), input_data.size(), result_data.data());

    for (unsigned i = 0; i < output_data.size(); i++) {
        ASSERT_EQ(output_data[i], result_data[i]);
    }
}

