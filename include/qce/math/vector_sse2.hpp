// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

// do not include manually
// used only in the composition of the math.hpp

#if defined(VECTOR_IMPLEMENTATION)
#  error "Vector already implemented in other file. Check includes!"
#endif

#define VECTOR_IMPLEMENTATION "sse2"

// TODO
