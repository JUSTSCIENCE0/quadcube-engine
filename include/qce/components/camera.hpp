// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

#include <cstring>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(CameraDirection) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_FORWARD) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_BACK) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_UP) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_DOWN) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_LEFT) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_RIGHT) \
    CU_END_ENUM(CameraDirection) \
    CU_BEGIN_ENUM(CameraType) \
        CU_ENUM_UNIT(E_CAMERA_FIXED) \
        CU_ENUM_UNIT(E_CAMERA_FIRST_PERSON) \
        CU_ENUM_UNIT(E_CAMERA_THIRD_PERSON) \
    CU_END_ENUM(CameraType)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct FirstPersonCameraControl {
        float3d view_rotation = {};
        bool    reset_view_rotation = false;
    };

    struct CameraView {
        float max_pitch_abs = deg_to_rad(89);

        bool is_LH = true;

        float3d position = { 2.0f, 2.0f, -2.0f };
        float3d target = { 0.0f, 0.0f, 0.0f };
        float3d up_direction = { 0.0f, 1.0f, 0.0f };

        bool    actual_fru = false;
        float3d forward = {};
        float3d right = {};
        float3d actual_up = {};

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
    };

    struct CameraProj {
        bool is_LH = true;

        float fov_rad = deg_to_rad(60);
        float aspect  = 16.0f / 9.0f;
        float znear   = 1.0f;
        float zfar    = 1000.0f;

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
    void camera_recalc_view(CameraView& camera_view);
    void camera_recalc_proj(CameraProj& camera_proj);

    void camera_move(CameraView& camera, const float3d& move);
    void camera_rotate_view(CameraView& camera, float d_yaw, float d_pitch);
}