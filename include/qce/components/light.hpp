// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

namespace QCE {
    struct DirectionalLight{
        color_rgba color = WHITE;
        float3d    direction = { 0.0f, -1.0f, 0.0f };
    };

    struct PointLight {
        color_rgba color = WHITE;
        float3d    position = { 0.0f, 2.0f, 0.0f };
        float falloff_begin = 1.0f;
        float falloff_end = 10.0f;
    };

    struct SpotLight {
        color_rgba color = WHITE;
        float3d    position = { 0.0f, 2.0f, 0.0f };
        float3d    direction = { 0.0f, -1.0f, 0.0f };
        float falloff_begin = 1.0f;
        float falloff_end = 10.0f;
        float spot_power = 64.0f;
    };
}
