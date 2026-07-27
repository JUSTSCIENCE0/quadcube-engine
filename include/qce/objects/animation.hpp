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
        CU_ENUM_UNIT(E_EASING_EASE_IN_QUAD) \
        CU_ENUM_UNIT(E_EASING_EASE_IN_EXPO) \
        CU_ENUM_UNIT(E_EASING_EASE_OUT_QUAD) \
        CU_ENUM_UNIT(E_EASING_EASE_OUT_SQRT) \
        CU_ENUM_UNIT(E_EASING_SMOOTH_STEP) \
        CU_ENUM_UNIT(E_EASING_SMOOTHER_STEP) \
    CU_END_ENUM(EasingFunc) \
    CU_BEGIN_ENUM(SplineFunc) \
        CU_ENUM_UNIT(E_SPLINE_LINEAR) \
        CU_ENUM_UNIT(E_SPLINE_CATMULL_ROM) \
    CU_END_ENUM(SplineFunc)
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
            quaternion value = { 0.0f, 0.0f, 0.0f, 1.0f };
            float start_time = 0.0f;

            EasingFunc easing =
                EasingFunc::E_EASING_LINEAR;
        };
        struct PositionKey {
            float3d value = { 0.0f, 0.0f, 0.0f };
            float start_time = 0.0f;
            EasingFunc easing =
                EasingFunc::E_EASING_LINEAR;
        };
        struct ScaleKey {
            float3d value = { 1.0f, 1.0f, 1.0f };
            float start_time = 0.0f;
            EasingFunc easing =
                EasingFunc::E_EASING_LINEAR;
        };

        std::string id{};
        std::vector<RotationKey> rotation_channel{};
        std::vector<PositionKey> position_channel{};
        std::vector<ScaleKey>    scale_channel{};
        float                    total_duration = 0.0f;

        SplineFunc spline_func = SplineFunc::E_SPLINE_LINEAR;

        // cache
        struct SplineCoeffs {
            float3d a;
            float3d b;
            float3d c;
        };
        std::vector<SplineCoeffs> spline_cache{};
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

    static inline void calculate_catmull_rom_spline(TransformAnimation& animation, bool is_looped) {
        assert(animation.position_channel.size() >= 4);
        animation.spline_cache.reserve(animation.position_channel.size());

        vector p0, p1, p2, p3;
        const auto last_index = animation.position_channel.size() - 1;

        for (size_t i = 0; i < last_index; i++) {
            auto p0_index = i - 1;
            auto p1_index = i;
            auto p2_index = i + 1;
            auto p3_index = i + 2;
            if (i == 0) {
                if (is_looped)
                    p0_index = last_index - 1;
                else
                    p0_index = 0;
            }
            if (p3_index > last_index) {
                if (is_looped)
                    p3_index = 1;
                else
                    p3_index = last_index;
            }

            p0 = vector_init(animation.position_channel[p0_index].value.arr);
            p1 = vector_init(animation.position_channel[p1_index].value.arr);
            p2 = vector_init(animation.position_channel[p2_index].value.arr);
            p3 = vector_init(animation.position_channel[p3_index].value.arr);

            auto a = (p1 - p2) * 1.5f + (p3 - p0) * 0.5f;
            auto b = p0 - p1 * 2.5f + p2 * 2.0f - p3 * 0.5f;
            auto c = (p2 - p0) * 0.5f;

            TransformAnimation::SplineCoeffs& coeffs = animation.spline_cache.emplace_back();
            vector_copy(a,  coeffs.a.arr);
            vector_copy(b,  coeffs.b.arr);
            vector_copy(c,  coeffs.c.arr);
        }
    }

    static inline void update_spline_cache(TransformAnimation& animation, bool is_looped = true) {
        animation.spline_cache.clear();

        if (animation.position_channel.empty())
            return;

        switch (animation.spline_func) {
        case SplineFunc::E_SPLINE_CATMULL_ROM:
            if (animation.position_channel.size() < 4) {
                // TODO: use log system
                std::cout << "Warning: Catmull-Rom requires at least 4 points!" << std::endl;
                return;
            }
            calculate_catmull_rom_spline(animation, is_looped);
            break;
        case SplineFunc::E_SPLINE_LINEAR:
            break;
        default:
            assert(!"Unsupported spline function");
            break;
        }
    }
}