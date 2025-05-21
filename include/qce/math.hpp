// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

// TODO: common-utils macro
// select implementation
#if defined(CU_COMPILE_UNIT_SSE2)
#  include <qce/math/vector_sse2.hpp>
#  include <qce/math/matrix_sse2.hpp>
#elif defined(CU_COMPILE_UNIT_AVX512)
#  include <qce/math/vector_avx512.hpp> // TODO: do we need it?
#  include <qce/math/matrix_avx512.hpp>
#else // CU_COMPILE_UNIT_DEF or just default implementation
#  include <qce/math/vector_def.hpp>
#  include <qce/math/matrix_def.hpp>
#endif


