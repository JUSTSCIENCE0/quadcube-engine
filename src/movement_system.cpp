// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/systems/movement_system.hpp>
#include <qce/ancillary/timer.hpp>

namespace QCE {
    ErrorCode MovementSystem::Update() {
        auto entities = m_entities.QueryEntities<Movement>();
        for (const auto& entity_id : entities) {
            auto& movement = m_entities.GetComponent<Movement>(entity_id);
            if (!movement.actual_velocity) {
                movement_recalc_velocity(movement);
            }
            auto translation = movement_calc_translation(movement, FrameTime::Get().Elapsed());

            if (m_entities.HasComponent<TransformComponents>(entity_id)) {
                // TODO: apply movement to transform
                // auto& transform = m_entities.GetComponent<TransformComponents>(entity_id);
            }

            if (m_entities.HasComponent<CameraComponents>(entity_id)) {
                auto& camera = m_entities.GetComponent<CameraComponents>(entity_id);
                camera_move(camera, translation);
            }

            if (movement.need_reset_direction) {
                movement.direction = {};
                movement.actual_velocity = true;
                movement.linear_velocity = {};
            }
        }

        return ErrorCode::SUCCESS;
    }
}
