// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>
#include <qce/renders/render_type.hpp>
#include <qce/hid/events_handler.hpp>

namespace QCE {
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
            const std::string& camera_name = "MainCamera",
            CameraType camera_type = E_CAMERA_FIRST_PERSON
        );

        ErrorCode Update();

    private:
        class MoveCommand : public BaseCommand {
        public:
            MoveCommand(
                    std::string name,
                    Entities& entities,
                    entity_id_t camera_id,
                    CameraDirection direction) :
                BaseCommand(std::move(name)),
                m_entities(entities),
                m_camera_id(camera_id),
                m_direction(direction) {
            }

        private:
            ErrorCode Execute(const CommandContext* context) override;

            Entities& m_entities;
            const entity_id_t m_camera_id;
            const CameraDirection m_direction;
        };

        class RotateCommand : public BaseCommand {
        public:
            RotateCommand(
                    std::string name,
                    Entities& entities,
                    entity_id_t camera_id,
                    CameraDirection direction) :
                BaseCommand(std::move(name)),
                m_entities(entities),
                m_camera_id(camera_id),
                m_direction(direction) {
            }

        private:
            ErrorCode Execute(const CommandContext* context) override;

            Entities& m_entities;
            const entity_id_t m_camera_id;
            const CameraDirection m_direction;
        };

        ErrorCode RegisterFpcEventHandlers(std::string camera_name, entity_id_t camera_id);
        // TODO: UnregisterEventHandlers

        static constexpr float MIN_FOV_DEG = 30.0f;

        Entities&  m_entities;
        RenderType m_render_type = DEFAULT_RENDER_TYPE;
    };
}
