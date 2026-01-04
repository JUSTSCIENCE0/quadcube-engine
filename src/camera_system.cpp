// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/systems/camera_system.hpp>

namespace QCE {
    ErrorCode CameraSystem::AddCamera(
            float aspect,
            float fov_rad,
            float znear,
            float zfar,
            const float3d& position,
            const float3d& target,
            const float3d& up,
            CameraType camera_type) {
        if (aspect <= 0.0f)
            return ErrorCode::E_ENG_WRONG_ASPECT;
        if (fov_rad < deg_to_rad(MIN_FOV_DEG))
            return ErrorCode::E_ENG_WRONG_FOV;
        if (zfar <= znear)
            return ErrorCode::E_ENG_WRONG_ZLIMITS;

        //const auto cameras_count = m_entities.QueryEntities<CameraComponents>().size();

        const bool is_LH_system = (m_render_type == RenderType::E_RENDER_DIRECTX12);
        const auto camera_entity_id = m_entities.AddEntity();

        if (camera_type != E_CAMERA_FIXED)
            QCE_CRITICAL(m_entities.AddComponent(camera_entity_id, Movement{}));

        TransformComponents tc{
            .position = position
        };
        QCE_CRITICAL(m_entities.AddComponent(camera_entity_id, tc));

        CameraComponents cc{
            .target = target,
            .up_direction = up,

            .fov_rad = fov_rad,
            .aspect = aspect,
            .znear = znear,
            .zfar = zfar,

            .is_LH = is_LH_system
        };
        QCE_CRITICAL(m_entities.AddComponent(camera_entity_id, cc));

        return ErrorCode::SUCCESS;
    }

    ErrorCode CameraSystem::Update() {


        return ErrorCode::SUCCESS;
    }
}
