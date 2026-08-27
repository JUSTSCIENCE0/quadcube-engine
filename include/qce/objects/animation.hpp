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
    CU_BEGIN_ENUM_TYPED(EasingFunc, int8_t) \
        CU_ENUM_UNIT(E_EASING_LINEAR) \
        CU_ENUM_UNIT(E_EASING_EASE_IN_QUAD) \
        CU_ENUM_UNIT(E_EASING_EASE_IN_EXPO) \
        CU_ENUM_UNIT(E_EASING_EASE_OUT_QUAD) \
        CU_ENUM_UNIT(E_EASING_EASE_OUT_SQRT) \
        CU_ENUM_UNIT(E_EASING_SMOOTH_STEP) \
        CU_ENUM_UNIT(E_EASING_SMOOTHER_STEP) \
        CU_ENUM_ANCILLARY_UNITS(E_EASING) \
    CU_END_ENUM(EasingFunc) \
    CU_BEGIN_ENUM_TYPED(SplineFunc, int8_t) \
        CU_ENUM_UNIT(E_SPLINE_LINEAR) \
        CU_ENUM_UNIT(E_SPLINE_CATMULL_ROM) \
        CU_ENUM_ANCILLARY_UNITS(E_SPLINE) \
    CU_END_ENUM(SplineFunc) \
    CU_BEGIN_ENUM_TYPED(FloatQuantization, int8_t) \
        CU_VALUED_ENUM_UNIT(E_0_BIT_FQ,  0) \
        CU_VALUED_ENUM_UNIT(E_8_BIT_FQ,  1) \
        CU_VALUED_ENUM_UNIT(E_16_BIT_FQ, 2) \
        CU_VALUED_ENUM_UNIT(E_24_BIT_FQ, 3) \
        CU_VALUED_ENUM_UNIT(E_NO_FQ,     4) \
    CU_END_ENUM(FloatQuantization) \
    CU_BEGIN_ENUM_TYPED(QuaternionQuantization, int8_t) \
        CU_VALUED_ENUM_UNIT(E_32_BIT_QQ, 0) \
        CU_VALUED_ENUM_UNIT(E_64_BIT_QQ, 1) \
        CU_VALUED_ENUM_UNIT(E_NO_QQ,     3) \
    CU_END_ENUM(QuaternionQuantization)
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

    // TODO: Explore possible optimization
    // Store animation channels and quaternions in memory in compressed form
    // This will definitely reduce RAM consumption and potentially optimize CPU cache usage
    // Presumably the second one provides a performance gain greater than the overhead of decompression
    // This needs to be verified using a benchmark

    struct AnimationRotationKey {
        quaternion value = { 0.0f, 0.0f, 0.0f, 1.0f };
        float start_time = 0.0f;

        EasingFunc easing =
            EasingFunc::E_EASING_LINEAR;
    };
    struct AnimationPositionKey {
        float3d value = { 0.0f, 0.0f, 0.0f };
        float start_time = 0.0f;
        EasingFunc easing =
            EasingFunc::E_EASING_LINEAR;
    };
    struct AnimationScaleKey {
        float3d value = { 1.0f, 1.0f, 1.0f };
        float start_time = 0.0f;
        EasingFunc easing =
            EasingFunc::E_EASING_LINEAR;
    };

    struct TransformAnimation {
        TransformAnimation() = default;
        TransformAnimation(const TransformAnimation&) = delete;
        TransformAnimation& operator=(const TransformAnimation&) = delete;
        TransformAnimation(TransformAnimation&&) noexcept = default;
        TransformAnimation& operator=(TransformAnimation&&) noexcept = default;

        std::string id{};
        std::vector<AnimationRotationKey> rotation_channel{};
        std::vector<AnimationPositionKey> position_channel{};
        std::vector<AnimationScaleKey>    scale_channel{};
        float total_duration = 0.0f;

        SplineFunc spline_func = SplineFunc::E_SPLINE_LINEAR;

        // cache
        struct SplineCoeffs {
            float3d a;
            float3d b;
            float3d c;
        };
        std::vector<SplineCoeffs> spline_cache{};
    };

    struct TransformAnimationCompressionContext {
        FloatQuantization position_quantization[3] = {
            E_NO_FQ, E_NO_FQ, E_NO_FQ };
        float3d position_min{};
        float3d position_max{};
        FloatQuantization scale_quantization[3] = {
            E_NO_FQ, E_NO_FQ, E_NO_FQ };
        float3d scale_min{};
        float3d scale_max{};
        QuaternionQuantization rotation_quantization = E_32_BIT_QQ;
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

    template<typename T>
    concept Float3dChannel =
        KeyChannel<T> &&
        requires(typename T::value_type key) {
            { key.value } -> std::convertible_to<float3d>;
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

    template<Float3dChannel T>
    static inline void calculate_min_max(
            const T& channel, float3d& min_value, float3d& max_value) {
        min_value = channel.front().value;
        max_value = channel.front().value;

        for (const auto& key : channel) {
            min_value.x() = std::min(min_value.x(), key.value.x());
            min_value.y() = std::min(min_value.y(), key.value.y());
            min_value.z() = std::min(min_value.z(), key.value.z());

            max_value.x() = std::max(max_value.x(), key.value.x());
            max_value.y() = std::max(max_value.y(), key.value.y());
            max_value.z() = std::max(max_value.z(), key.value.z());
        }
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

    static inline FloatQuantization calculate_float_quantization(float min_value, float max_value, float eps) {
        if (eps <= 0.0f)
            return FloatQuantization::E_NO_FQ;

        assert(min_value <= max_value);
        float range = max_value - min_value;
        if (range <= eps)
            return FloatQuantization::E_0_BIT_FQ;

        float quantization_levels = range / eps;
        if (quantization_levels <= 256.0f)
            return FloatQuantization::E_8_BIT_FQ;
        if (quantization_levels <= 65536.0f)
            return FloatQuantization::E_16_BIT_FQ;
        if (quantization_levels <= 16777216.0f)
            return FloatQuantization::E_24_BIT_FQ;
        return FloatQuantization::E_NO_FQ;
    }

    static inline TransformAnimationCompressionContext calculate_compression_context(
            const TransformAnimation& animation,
            float position_eps = 0.0005f,
            float scale_eps    = 0.0005f,
            QuaternionQuantization qq = E_32_BIT_QQ) {
        TransformAnimationCompressionContext context{};
        if (!animation.position_channel.empty()) {
            calculate_min_max(animation.position_channel, context.position_min, context.position_max);
        }
        if (!animation.scale_channel.empty()) {
            calculate_min_max(animation.scale_channel, context.scale_min, context.scale_max);
        }
        return context;
    }
}