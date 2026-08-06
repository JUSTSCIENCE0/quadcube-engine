// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>

namespace QCE {
    template<typename S>
    void serialize(S& s, float3d& value) {
        s.value4b(value.x());
        s.value4b(value.y());
        s.value4b(value.z());
    }

    template<typename S>
    void serialize(S& s, quaternion& value) {
        // TODO: Compression of quaternion
        // 128 bit -> 32 bit (2 + 3 * 10)
        s.value4b(value.x());
        s.value4b(value.y());
        s.value4b(value.z());
        s.value4b(value.w());
    }
}
