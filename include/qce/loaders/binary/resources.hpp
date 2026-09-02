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

        ser.text1b(animation.id, MAX_ID_LENGTH);

        ser.container(animation.rotation_channel, MAX_ANIMATION_KEYS_COUNT);

        Float3dChannelCompressor compressor { compression_ctx };
        if (compression_ctx.has_position_channel) {
            ser.container(
                animation.position_channel,
                MAX_ANIMATION_KEYS_COUNT,
                compressor);
        }
        ser.container(animation.scale_channel,    MAX_ANIMATION_KEYS_COUNT);

        ser.value4b(animation.total_duration); // TODO: use total duration as context to compress start_time of keys
        ser.value1b(animation.spline_func);
    }

    template<typename Des, typename Fnc>
    void deserialize(Des& des, QCE::TransformAnimation& animation, Fnc&&) const {
        // TODO;
    }

private:
    struct Float3dChannelCompressor {
        template<typename S, typename T>
        void operator()(S& sbp, T& key) {
            for (int i = 0; i < 3; i++) {
                if (ctx.position_quantization[0] != QCE::E_NO_FQ) {
                    const auto RANGE_SPEC = bitsery::ext::ValueRange<float>(
                        ctx.position_min.arr[i],
                        ctx.position_max.arr[i],
                        bitsery::ext::BitsConstraint(ctx.position_quantization[i] * 8));
                    sbp.ext(key.value.arr[i], RANGE_SPEC);
                }
                else {
                    sbp.value4b(key.value.arr[i]);
                }
            }
            sbp.value4b(key.start_time); // TODO: use context to compress start_time
            sbp.value1b(key.easing);
        }

        QCE::TransformAnimationCompressionContext ctx;
    };
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
