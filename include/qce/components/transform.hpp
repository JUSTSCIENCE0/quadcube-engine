// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

namespace QCE {
    using quaternion = float4d;

    struct TransformComponents {
        quaternion rotation = { 0.0f, 0.0f, 0.0f };
        float3d    position = { 0.0f, 0.0f, 0.0f };
        float3d    scale    = { 1.0f, 1.0f, 1.0f };
    };

    struct TransformMatrix {
        bool actual = false;

        float4x4 data{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            }
        };

        float4x4 transposed_data{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            }
        };
    };

    // helpers
    void normalize_quaternion(quaternion& quat);
    void calculate_transform_matrix(const TransformComponents& components, TransformMatrix& out);
    void decompose_transform_matrix(const TransformMatrix& matrix, TransformComponents& out);
}
