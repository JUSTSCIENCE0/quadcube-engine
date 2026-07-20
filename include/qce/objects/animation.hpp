// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>
#include <qce/ancillary/error_codes.hpp>

#include <vector>
#include <concepts>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(EasingFunc) \
        CU_ENUM_UNIT(E_EASING_LINEAR) \
        CU_ENUM_UNIT(E_EASING_EASE_IN) \
        CU_ENUM_UNIT(E_EASING_EASE_OUT) \
        CU_ENUM_UNIT(E_EASING_EASE_IN_OUT) \
    CU_END_ENUM(EasingFunc) \
    CU_BEGIN_ENUM(PathInterpolationFunc) \
        CU_ENUM_UNIT(E_PATH_FUNC_LINEAR) \
        CU_ENUM_UNIT(E_PATH_FUNC_CATMULL_ROM) \
        CU_ENUM_UNIT(E_PATH_FUNC_BEZIER) \
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

            EasingFunc interpolation_func =
                EasingFunc::E_EASING_LINEAR;
        };
        struct PositionKey {
            float3d position = { 0.0f, 0.0f, 0.0f };
            float start_time = 0.0f;
            EasingFunc interpolation_func =
                EasingFunc::E_EASING_LINEAR;
        };
        struct ScaleKey {
            float3d scale = { 1.0f, 1.0f, 1.0f };
            float start_time = 0.0f;
            EasingFunc interpolation_func =
                EasingFunc::E_EASING_LINEAR;
        };

        std::string id{};
        std::vector<RotationKey> rotation_channel{};
        std::vector<PositionKey> position_channel{};
        std::vector<ScaleKey>    scale_channel{};
        float                    total_duration = 0.0f;

        PathInterpolationFunc path_interpolation_func =
            PathInterpolationFunc::E_PATH_FUNC_LINEAR;
    };

    static inline float  calculate_animation_duration(const TransformAnimation& animation) {
        float max_duration = 0.0f;
        if (!animation.rotation_channel.empty())
            max_duration = animation.rotation_channel.back().start_time;
        if (!animation.position_channel.empty())
            max_duration = std::max(max_duration, animation.position_channel.back().start_time);
        if (!animation.scale_channel.empty())
            max_duration = std::max(max_duration, animation.scale_channel.back().start_time);
        return max_duration;
    }

    template<typename T>
    concept KeyChannel =
        requires { typename T::value_type; } &&
        std::same_as<T, std::vector<typename T::value_type>> &&
        requires(typename T::value_type key) {
            { key.start_time } -> std::convertible_to<float>;
        };

    template<KeyChannel T>
    static inline ErrorCode validate_key_channel(const T& channel, float max_duration) {
        for (size_t i = 1; i < channel.size(); i++) {
            const auto& prev_time = channel[i - 1].start_time;
            const auto& curr_time = channel[i].start_time;
            if (prev_time >= curr_time)
                return ErrorCode::E_RM_ANIMATION_INVALID_TIMELINE;
            if (curr_time > max_duration)
                return ErrorCode::E_RM_ANIMATION_WRONG_DURATION;
        }
        return ErrorCode::SUCCESS;
    }

    static inline ErrorCode validate_animation(const TransformAnimation& animation) {
        if (animation.rotation_channel.empty() &&
            animation.position_channel.empty() &&
            animation.scale_channel.empty())
            return ErrorCode::E_RM_ANIMATION_EMPTY;

        QCE_CRITICAL(validate_key_channel(animation.rotation_channel, animation.total_duration));
        QCE_CRITICAL(validate_key_channel(animation.position_channel, animation.total_duration));
        QCE_CRITICAL(validate_key_channel(animation.scale_channel, animation.total_duration));

        return ErrorCode::SUCCESS;
    }
}