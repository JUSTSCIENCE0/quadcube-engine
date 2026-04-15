// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include "configs_implementation.hpp"

#include <qce/ecs/ecs.hpp>
#include <qce/hid/events_handler.hpp>

namespace QCE {
    class CameraSystem {
    public:
        using Config = CameraConfig;

        explicit CameraSystem(Entities& entities) :
            m_entities(entities)
        {}

        ErrorCode Setup(Config config);
        ErrorCode Setup();

        ErrorCode AddCamera(const CameraConfigUnit& config);

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

        Entities& m_entities;
        Config    m_config{};
    };
}
