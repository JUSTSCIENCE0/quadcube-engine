// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>
#include <qce/renders/render_type.hpp>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(CameraDirection) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_FORWARD) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_BACK) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_UP) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_DOWN) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_LEFT) \
        CU_ENUM_UNIT(E_CAMERA_DIRECTION_RIGHT) \
    CU_END_ENUM(CameraDirection) \
    CU_BEGIN_ENUM(CameraType) \
        CU_ENUM_UNIT(E_CAMERA_FIXED) \
        CU_ENUM_UNIT(E_CAMERA_FIRST_PERSON) \
        CU_ENUM_UNIT(E_CAMERA_THIRD_PERSON) \
    CU_END_ENUM(CameraType)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    class CameraSystem {
    public:
        explicit CameraSystem(Entities& entities) :
            m_entities(entities)
        {}

        ErrorCode Setup(RenderType render_type) {
            m_render_type = render_type;
            // TODO: reconfigure cameras if needed
            return ErrorCode::SUCCESS;
        }

        ErrorCode AddCamera(
            float aspect = (16.0f / 9.0f),
            float fov_rad = deg_to_rad(60),
            float znear = 1.0f,
            float zfar = 1000.0f,
            const float3d& position = { 2.0f, 2.0f, -2.0f },
            const float3d& target = { 0.0f, 0.0f, 0.0f },
            const float3d& up = { 0.0f, 1.0f, 0.0f },
            CameraType camera_type = E_CAMERA_FIRST_PERSON
        );

        ErrorCode Update();

    private:
        static constexpr float MIN_FOV_DEG = 30.0f;

        Entities&  m_entities;
        RenderType m_render_type = DEFAULT_RENDER_TYPE;
    };
}
