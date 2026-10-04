// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/loaders/binary/resources.hpp>
#include <qce/objects/scene.hpp>

#include <bitsery/brief_syntax/variant.h>

namespace QCE {
    template<typename S>
    void serialize(S& s, ResourceDescription& resource) {
        s.object(resource.name);
        s.object(resource.params);
    }

    template<typename S>
    void serialize(S& s, SceneDescription& scene) {
        s.object(scene.resources);
    }
}
