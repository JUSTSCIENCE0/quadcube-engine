// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

#include <vector>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(InterpolationFunc) \
        CU_ENUM_UNIT(E_INTERPOLATION_LINEAR) \
        CU_ENUM_UNIT(E_INTERPOLATION_EASE_IN) \
        CU_ENUM_UNIT(E_INTERPOLATION_EASE_OUT) \
        CU_ENUM_UNIT(E_INTERPOLATION_EASE_IN_OUT) \
    CU_END_ENUM(InterpolationFunc) \
    CU_BEGIN_ENUM(PathInterpolationFunc) \
        CU_ENUM_UNIT(E_PATH_INTERPOLATION_LINEAR) \
        CU_ENUM_UNIT(E_PATH_INTERPOLATION_CATMULL_ROM) \
        CU_ENUM_UNIT(E_PATH_INTERPOLATION_BEZIER) \
    CU_END_ENUM(PathInterpolationFunc)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct TransformAnimationComponent {
        size_t index = 0;
        bool   is_looped = false;

        size_t current_rotation_key  = 0;
        size_t current_position_key = 0;
        size_t current_scale_key    = 0;
        float  current_time = 0.0f;
    };

    struct TransformAnimation {
        TransformAnimation() = default;
        TransformAnimation(const TransformAnimation&) = delete;
        TransformAnimation& operator=(const TransformAnimation&) = delete;
        TransformAnimation(TransformAnimation&&) noexcept = default;
        TransformAnimation& operator=(TransformAnimation&&) noexcept = default;

        struct RotationKey {
            quaternion rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
            float start_time = 0.0f;

            InterpolationFunc interpolation_func =
                InterpolationFunc::E_INTERPOLATION_LINEAR;
        };
        struct PositionKey {
            float3d position = { 0.0f, 0.0f, 0.0f };
            float start_time = 0.0f;
            InterpolationFunc interpolation_func =
                InterpolationFunc::E_INTERPOLATION_LINEAR;
        };
        struct ScaleKey {
            float3d scale = { 1.0f, 1.0f, 1.0f };
            float start_time = 0.0f;
            InterpolationFunc interpolation_func =
                InterpolationFunc::E_INTERPOLATION_LINEAR;
        };

        std::string id{};
        std::vector<RotationKey> rotation_channel{};
        std::vector<PositionKey> position_channel{};
        std::vector<ScaleKey>    scale_channel{};

        PathInterpolationFunc path_interpolation_func =
            PathInterpolationFunc::E_PATH_INTERPOLATION_LINEAR;
    };
}