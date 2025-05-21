// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <gtest/gtest.h>

#include "vector_operations.hpp"

int main(int argc, char* argv[]) {
    QCE::vector_addition_def(nullptr, 0, nullptr);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}