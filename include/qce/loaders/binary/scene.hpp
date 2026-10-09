// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/loaders/binary/resources.hpp>
#include <qce/loaders/binary/components.hpp>

#include <bitsery/brief_syntax/variant.h>

namespace QCE {
    template<typename S>
    void serialize(S& s, ResourceDescription& resource) {
        s.object(resource.name);
        s.object(resource.params);
    }

    template<typename S>
    void serialize(S& s, EntityDescription& entity) {
        s.object(entity.name);
        s.object(entity.static_mesh);
        s.object(entity.dynamic_mesh);
        s.object(entity.material);
        s.object(entity.directional_light);
        s.object(entity.point_light);
        s.object(entity.spot_light);
        s.object(entity.transform);
        s.object(entity.transform_animation);
    }

    template<typename S>
    void serialize(S& s, SceneDescription& scene) {
        s.object(scene.resources);
        s.object(scene.entities);
    }
}
