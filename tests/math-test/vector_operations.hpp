// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

// TODO: autogenerate this file

#include <cu/simd-utils.hpp>

#if 1 // CU_SIMD_SUPPORT_DEF
#define CU_SIMD_IFACE(name) CU_CONCAT(name, def)
#include "vector_operations_iface.hpp"
#undef CU_SIMD_IFACE
#endif

#if 0 // CU_SIMD_SUPPORT_SSE
#define CU_SIMD_IFACE(name) CU_CONCAT(name, sse)
#include "vector_operations_iface.hpp"
#undef CU_SIMD_IFACE
#endif

#if 1 // CU_SIMD_SUPPORT_SSE2
#define CU_SIMD_IFACE(name) CU_CONCAT(name, sse2)
#include "vector_operations_iface.hpp"
#undef CU_SIMD_IFACE
#endif

// TODO: etc

#if 0 // CU_SIMD_SUPPORT_AVX512
#define CU_SIMD_IFACE(name) CU_CONCAT(name, avx512)
#include "vector_operations_iface.hpp"
#undef CU_SIMD_IFACE
#endif
