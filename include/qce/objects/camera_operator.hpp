// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/camera.hpp>
#include <qce/hid/events_handler.hpp>

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
    CU_BEGIN_ENUM(CameraOperatorType) \
        CU_ENUM_UNIT(E_CAMERA_OPERATOR_FIXED) \
        CU_ENUM_UNIT(E_CAMERA_OPERATOR_FIRST_PERSON) \
        CU_ENUM_UNIT(E_CAMERA_OPERATOR_THIRD_PERSON) \
    CU_END_ENUM(CameraOperatorType)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    class CameraOperator {
    public:
        explicit CameraOperator(std::string id, Camera& camera):
            m_name(std::move(id)),
            m_camera(camera) {}
        virtual ~CameraOperator() = default;
    protected:
        const std::string m_name;
        Camera& m_camera;
    };

    class FirstPersonCameraOperator: public CameraOperator {
    public:
        explicit FirstPersonCameraOperator(std::string id, Camera& camera)
            : CameraOperator(std::move(id), camera) {}

    private:
        class MoveCommand : public Command {
            MoveCommand(std::string name, CameraDirection direction, FirstPersonCameraOperator& fpco):
                Command(std::move(name)),
                m_fpco(fpco),
                m_direction(direction) {}

        private:
            ErrorCode Execute(const CommandContext* context) override;

            FirstPersonCameraOperator& m_fpco;
            const CameraDirection m_direction;
        };

        void RegisterEventHandlers() {
            // TODO
        }
    };
}
