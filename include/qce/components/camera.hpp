// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

namespace QCE {
    struct CameraComponents {
        float max_pitch_abs  = deg_to_rad(89);

        float3d position     = { 2.0f, 2.0f, -2.0f };
        float3d target       = { 0.0f, 0.0f, 0.0f };
        float3d up_direction = { 0.0f, 1.0f, 0.0f };

        float fov_rad = deg_to_rad(60);
        float aspect  = 16.0f / 9.0f;
        float znear   = 1.0f;
        float zfar    = 1000.0f;

        bool is_LH = true;

        bool    actual_fru = false;
        float3d forward    = {};
        float3d right      = {};
        float3d actual_up  = {};

        bool  actual_yaw_pitch = false;
        float yaw = 0.0f;
        float pitch = 0.0f;

        bool actual_view = false;
        float4x4 view{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            }
        };

        bool actual_proj = false;
        float4x4 proj{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 0.0f
            }
        };
    };

    // helpers
    void camera_recalc_view(CameraComponents& camera);
    void camera_recalc_proj(CameraComponents& camera);

    void camera_move(CameraComponents& camera, const float3d& move);
    void camera_rotate_view(CameraComponents& camera, float d_yaw, float d_pitch);
}