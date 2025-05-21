// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <stdint.h>

// TODO: move this block to common-utils
// -------------------------------------
#if defined(CU_COMPILE_UNIT_SSE2)
#  define FUNC_POSTFIX sse2
#elif defined(CU_COMPILE_UNIT_AVX512)
#  define FUNC_POSTFIX avx512
#else // CU_COMPILE_UNIT_DEF or just default implementation
#  define FUNC_POSTFIX def
#endif

#define CU_CONCAT(lhs, rhs) lhs ##_ ## rhs
#define CU_EXPAND_CONCAT(lhs, rhs) CU_CONCAT(lhs, rhs)
#define CU_SIMD(name) CU_EXPAND_CONCAT(name, FUNC_POSTFIX)

// -------------------------------------
// TODO: move this block to common-utils

namespace QCE {
    void CU_CONCAT(vector_addition, def)(const float* values, int64_t count, float* results);
}
