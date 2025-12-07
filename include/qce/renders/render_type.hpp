// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(RenderType) \
        CU_ENUM_UNIT(E_RENDER_DIRECTX12) \
        CU_ENUM_UNIT(E_RENDER_VULKAN) \
    CU_END_ENUM(RenderType)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

#ifdef WIN32
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_DIRECTX12;
#else
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_VULKAN;
#endif
}