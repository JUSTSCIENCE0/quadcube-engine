// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#ifdef QCE_ECS_ADDITIONAL_COMPONENTS_HEADER
#  include QCE_ECS_ADDITIONAL_COMPONENTS_HEADER
#else
#  define QCE_ECS_ENTITY_MANAGER_CACHE_SIZE 64
#  define QCE_ECS_ADDITIONAL_COMPONENTS
#endif

#include <qce/ecs/entity_manager.hpp>

#include <qce/components/common.hpp>
#include <qce/components/camera.hpp>
#include <qce/components/movement.hpp>
#include <qce/components/transform.hpp>

namespace QCE {
    template <typename... AdditionalComponents>
    class EcsCore :
        public EntityManager<
            QCE_ECS_ENTITY_MANAGER_CACHE_SIZE,
            // Components
            EntityName,
            TransformComponents,
            TransformMatrix,
            Movement,
            CameraView,
            CameraProj,
            FirstPersonCameraControl,
            AdditionalComponents... > {};

    using Entities = EcsCore< QCE_ECS_ADDITIONAL_COMPONENTS >;
}
