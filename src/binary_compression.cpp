// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/loaders/binary/compression.hpp>
#include <qce/objects/animation.hpp>

namespace QCE {
    template<typename T>
    concept Float3dChannel =
        KeyChannel<T> &&
        requires(typename T::value_type key) {
            { key.value } -> std::convertible_to<float3d>;
    };

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

    static inline void calculate_float_quantization(
        const float3d& min_value, const float3d& max_value, float eps,
        FloatQuantization* quantization) {
        assert(quantization);
        quantization[0] = calculate_float_quantization(min_value.x(), max_value.x(), eps);
        quantization[1] = calculate_float_quantization(min_value.y(), max_value.y(), eps);
        quantization[2] = calculate_float_quantization(min_value.z(), max_value.z(), eps);
    }

    TransformAnimationCompressionContext calculate_compression_context(
            const TransformAnimation& animation,
            float position_eps, float scale_eps,
            QuaternionQuantization qq) {
        TransformAnimationCompressionContext context{};
        context.has_position_channel = !animation.position_channel.empty();
        if (context.has_position_channel) {
            calculate_min_max(animation.position_channel, context.position_min, context.position_max);
            calculate_float_quantization(
                context.position_min, context.position_max, position_eps, context.position_quantization);
        }
        context.has_scale_channel = !animation.scale_channel.empty();
        if (context.has_scale_channel) {
            calculate_min_max(animation.scale_channel, context.scale_min, context.scale_max);
            calculate_float_quantization(
                context.scale_min, context.scale_max, scale_eps, context.scale_quantization);
        }
        context.has_rotation_channel = !animation.rotation_channel.empty();
        if (context.has_rotation_channel) {
            context.rotation_quantization = qq;
        }
        return context;
    }
}
