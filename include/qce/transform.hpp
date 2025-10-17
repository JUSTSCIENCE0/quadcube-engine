// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math.hpp>

namespace QCE {
    class Transform {
    public:

    private:
        float4x4 transform_matrix{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            }
        };

        float4d rotation{};
        float3d position{};
        float3d scale = { 1.0f, 1.0f, 1.0f };
    };
}
