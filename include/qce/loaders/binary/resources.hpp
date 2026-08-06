// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/loaders/binary/math.hpp>
#include <qce/objects/animation.hpp>

#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <limits>

namespace QCE {
    static constexpr size_t MAX_ID_LENGTH = 256;
    static constexpr size_t MAX_ANIMATION_KEYS_COUNT = 1024;

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
    void serialize(S& s, TransformAnimation& anim) {
        s.text1b(anim.id, MAX_ID_LENGTH);

        // TODO: calc min and max position and scale coordinates and use them as context to compress values
        s.container(anim.rotation_channel, MAX_ANIMATION_KEYS_COUNT);
        s.container(anim.position_channel, MAX_ANIMATION_KEYS_COUNT);
        s.container(anim.scale_channel,    MAX_ANIMATION_KEYS_COUNT);

        s.value4b(anim.total_duration); // TODO: use total duration as context to compress start_time of keys
        s.value1b(anim.spline_func);
    }
}
