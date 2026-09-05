// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/animation.hpp>
#include <qce/loaders/binary/math.hpp>
#include <qce/loaders/binary/compression.hpp>

#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/ext/value_range.h>

#include <limits>

namespace bitsery {
namespace ext {

class TransformAnimationCompressor {
    static constexpr size_t MAX_ID_LENGTH = 256;
    static constexpr size_t MAX_ANIMATION_KEYS_COUNT = 1024;

public:
    template<typename Ser, typename Fnc>
    void serialize(Ser& ser, const QCE::TransformAnimation& animation, Fnc&&) const {
        auto compression_ctx = QCE::calculate_compression_context(animation);
        write_context_flags(ser, compression_ctx);
        process_context_floats(ser, compression_ctx);

        process_animation(ser, const_cast<QCE::TransformAnimation&>(animation), compression_ctx);
    }

    template<typename Des, typename Fnc>
    void deserialize(Des& des, QCE::TransformAnimation& animation, Fnc&&) const {
        QCE::TransformAnimationCompressionContext compression_ctx{};
        read_context_flags(des, compression_ctx);
        process_context_floats(des, compression_ctx);

        process_animation(des, animation, compression_ctx);
    }

private:
    class Float3dChannelCompressor {
    public:
        template<typename S, typename T>
        void operator()(S& sbp, T& key) {
            for (int i = 0; i < 3; i++) {
                if (m_quantization[i] != QCE::E_NO_FQ) {
                    const auto RANGE_SPEC = bitsery::ext::ValueRange<float>(
                        m_min.arr[i], m_max.arr[i],
                        bitsery::ext::BitsConstraint(m_quantization[i] * 8));
                    sbp.ext(key.value.arr[i], RANGE_SPEC);
                }
                else {
                    sbp.value4b(key.value.arr[i]);
                }
            }
            sbp.value4b(key.start_time); // TODO: use context to compress start_time
            sbp.value1b(key.easing);
        }

        void SetCtx(const QCE::float3d& min, const QCE::float3d& max, const QCE::FloatQuantization* q) {
            assert(q);

            m_min = min;
            m_max = max;
            std::memcpy(m_quantization, q, sizeof(m_quantization));
        }

    private:
        QCE::float3d m_min{};
        QCE::float3d m_max{};
        QCE::FloatQuantization m_quantization[3]{};
    };

    template<typename Ser>
    void write_context_flags(Ser& ser, const QCE::TransformAnimationCompressionContext& compression_ctx) const {
        auto write = [&ser](auto value, size_t Bits) {
            using T = decltype(value);
            ser.adapter().template writeBits<T>(value, Bits);
        };

        write(compression_ctx.has_position_channel ? 1u : 0u,    1);
        write(uint8_t(compression_ctx.position_quantization[0]), 3);
        write(uint8_t(compression_ctx.position_quantization[1]), 3);
        write(uint8_t(compression_ctx.position_quantization[2]), 3);

        write(compression_ctx.has_scale_channel ? 1u : 0u,    1);
        write(uint8_t(compression_ctx.scale_quantization[0]), 3);
        write(uint8_t(compression_ctx.scale_quantization[1]), 3);
        write(uint8_t(compression_ctx.scale_quantization[2]), 3);

        write(compression_ctx.has_rotation_channel ? 1u : 0u, 1);
        write(uint8_t(compression_ctx.rotation_quantization), 3);
    }

    template<typename Des>
    void read_context_flags(Des& des, QCE::TransformAnimationCompressionContext& compression_ctx) const {
        auto read = [&des](auto& value, size_t Bits) {
            des.adapter().readBits(reinterpret_cast<uint8_t&>(value), Bits);
        };

        read(compression_ctx.has_position_channel,     1);
        read(compression_ctx.position_quantization[0], 3);
        read(compression_ctx.position_quantization[1], 3);
        read(compression_ctx.position_quantization[2], 3);

        read(compression_ctx.has_scale_channel,     1);
        read(compression_ctx.scale_quantization[0], 3);
        read(compression_ctx.scale_quantization[1], 3);
        read(compression_ctx.scale_quantization[2], 3);

        read(compression_ctx.has_rotation_channel,  1);
        read(compression_ctx.rotation_quantization, 3);
    }

    template<typename S>
    void process_context_floats(S& s, QCE::TransformAnimationCompressionContext& compression_ctx) const {
        if (compression_ctx.has_position_channel) {
            for (int i = 0; i < 3; i++) {
                if (compression_ctx.position_quantization[i] != QCE::E_NO_FQ) {
                    s.value4b(compression_ctx.position_min.arr[i]);
                    s.value4b(compression_ctx.position_max.arr[i]);
                }
            }
        }

        if (compression_ctx.has_scale_channel) {
            for (int i = 0; i < 3; i++) {
                if (compression_ctx.scale_quantization[i] != QCE::E_NO_FQ) {
                    s.value4b(compression_ctx.scale_min.arr[i]);
                    s.value4b(compression_ctx.scale_max.arr[i]);
                }
            }
        }
    }

    template<typename S>
    void process_animation(
            S& s,
            QCE::TransformAnimation& animation,
            const QCE::TransformAnimationCompressionContext& compression_ctx) const {
        s.text1b(animation.id, MAX_ID_LENGTH);

        if (compression_ctx.has_rotation_channel) {
            QuaternionCompressor compressor{};
            compressor.quantization = compression_ctx.rotation_quantization;
            s.container(animation.rotation_channel, MAX_ANIMATION_KEYS_COUNT, 
                [&compressor](S& sl, auto& key) {
                    sl.ext(key.value, compressor);
                    sl.value4b(key.start_time);
                    sl.value1b(key.easing);
                });
        }

        Float3dChannelCompressor compressor{};
        if (compression_ctx.has_position_channel) {
            compressor.SetCtx(
                compression_ctx.position_min,
                compression_ctx.position_max,
                compression_ctx.position_quantization);
            s.container(
                animation.position_channel,
                MAX_ANIMATION_KEYS_COUNT,
                compressor);
        }
        if (compression_ctx.has_scale_channel) {
            compressor.SetCtx(
                compression_ctx.scale_min,
                compression_ctx.scale_max,
                compression_ctx.scale_quantization);
            s.container(
                animation.scale_channel,
                MAX_ANIMATION_KEYS_COUNT,
                compressor);
        }

        s.value4b(animation.total_duration); // TODO: use total duration as context to compress start_time of keys
        s.value1b(animation.spline_func);
    }
};

} // ext

namespace traits {

template<typename T>
struct ExtensionTraits<ext::TransformAnimationCompressor, T> {
    using TValue = void;
    static constexpr bool SupportValueOverload = false;
    static constexpr bool SupportObjectOverload = true;
    static constexpr bool SupportLambdaOverload = false;
};

} // traits
} // bitsery

namespace QCE {
    template<typename S>
    void serialize(S& s, AnimationRotationKey& key) {
        s.object(key.value);
        s.value4b(key.start_time); // TODO: use context to compress start_time
        s.value1b(key.easing);
    }

    template<typename S>
    void serialize(S& s, AnimationPositionKey& key) {
        s.object(key.value); // TODO: use context to compress value
        s.value4b(key.start_time); // TODO: use context to compress start_time
        s.value1b(key.easing);
    }

    template<typename S>
    void serialize(S& s, AnimationScaleKey& key) {
        s.object(key.value); // TODO: use context to compress value
        s.value4b(key.start_time); // TODO: use context to compress start_time
        s.value1b(key.easing);
    }

    template<typename S>
    void serialize(S& s, TransformAnimation& animation) {
        s.enableBitPacking([&animation](typename S::BPEnabledType& sbp) {
            sbp.ext(animation, bitsery::ext::TransformAnimationCompressor{});
        });
    }
}
