// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <cu/simd-utils.hpp>

#include <stdint.h>

namespace QCE {
    void CU_CONCAT(vector_addition, def)(const float* values, int64_t count, float* results);
}
