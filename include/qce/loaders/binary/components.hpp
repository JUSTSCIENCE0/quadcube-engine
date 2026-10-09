// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/scene.hpp>
#include <qce/loaders/binary/math.hpp>

#include <bitsery/brief_syntax/string.h>

namespace QCE {
    template<typename S>
    void serialize(S& s, DynamicMeshDescription& description) {
        s.object(description.base);
        s.object(description.deformated);
    }

    template<typename S>
    void serialize(S& s, TransformAnimationDescription& description) {
        s.object(description.animation_name);
        s.value1b(description.is_looped);
    }

    template<typename S>
    void serialize(S& s, DirectionalLight& light) {
        s.object(light.color);
        s.object(light.direction);
    }

    template<typename S>
    void serialize(S& s, PointLight& light) {
        s.object(light.color);
        s.object(light.position);
        s.value4b(light.falloff_begin);
        s.value4b(light.falloff_end);
    }

    template<typename S>
    void serialize(S& s, SpotLight& light) {
        s.object(light.color);
        s.object(light.position);
        s.object(light.direction);
        s.value4b(light.falloff_begin);
        s.value4b(light.falloff_end);
        s.value4b(light.spot_power);
    }

    template<typename S>
    void serialize(S& s, TransformComponents& components) {
        s.object(components.rotation);
        s.object(components.position);
        s.object(components.scale);
    }
}
