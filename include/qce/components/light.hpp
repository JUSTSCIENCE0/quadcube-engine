// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(LightType) \
        CU_ENUM_UNIT(E_LIGHT_TYPE_DIRECTIONAL) \
        CU_ENUM_UNIT(E_LIGHT_TYPE_POINT) \
        CU_ENUM_UNIT(E_LIGHT_TYPE_SPOT) \
    CU_END_ENUM(LightType)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct Light {
        LightType type = LightType::E_LightType_UNKNOWN;

        color_rgba color     = WHITE;
        float3d    direction = { 0.0f, -1.0f, 0.0f };
        float3d    position  = { 0.0f, 2.0f, 0.0f };

        float falloff_begin = 1.0f;
        float falloff_end   = 10.0f;
        float spot_power    = 64.0f;
    };
}
