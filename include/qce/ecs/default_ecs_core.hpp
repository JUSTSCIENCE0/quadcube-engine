// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/entity_manager.hpp>

#include <qce/components/camera.hpp>
#include <qce/components/movement.hpp>
#include <qce/components/transform.hpp>

namespace QCE {
    using Entities = EntityManager<
        64, // Cache Limit
        // Components
        TransformComponents,
        TransformMatrix,
        Movement,
        CameraComponents
    >;
}
