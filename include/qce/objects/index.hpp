// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <stdint.h>

#ifndef QCE_INDEX_WIDTH // TODO: doc & cmake configurable
#  define QCE_INDEX_WIDTH INDEX_WIDTH_4
#endif

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(IndexWidth) \
        CU_VALUED_ENUM_UNIT(INDEX_WIDTH_1, 1) \
        CU_VALUED_ENUM_UNIT(INDEX_WIDTH_2, 2) \
        CU_VALUED_ENUM_UNIT(INDEX_WIDTH_4, 4) \
    CU_END_ENUM(IndexWidth)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    namespace PrivateImplementation {
        template<int WIDTH> struct IndexTypeCalc;

        template<> struct IndexTypeCalc<INDEX_WIDTH_1> { using type = uint8_t; };
        template<> struct IndexTypeCalc<INDEX_WIDTH_2> { using type = uint16_t; };
        template<> struct IndexTypeCalc<INDEX_WIDTH_4> { using type = uint32_t; };
    }

    using index_t = PrivateImplementation::IndexTypeCalc<QCE_INDEX_WIDTH>::type;

    static_assert(sizeof(index_t) == QCE_INDEX_WIDTH, "Invalid index_t width");
}
