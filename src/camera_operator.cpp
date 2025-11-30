// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/camera_operator.hpp>

namespace QCE {
    ErrorCode FirstPersonCameraOperator::MoveCommand::Execute(const CommandContext* context) {
        assert(context);
        switch (context->type) {
        case CommandContextType::E_CCT_HID_EVENT: {
            auto hid_event = static_cast<const HidEvent*>(context);
            auto event_code = hid_event->GetCode();
            auto is_button = hid_event_is_button(event_code);
            if (is_button) {
                bool is_down = hid_event->IsButtonDown();
                if (is_down) {
                    std::cout << to_string(m_direction) << std::endl;
                    // TODO: set movement state
                }
                else {
                    // TODO: reset movement state
                }
            }
            else {
                bool has_displacement = hid_event_has_displacement(event_code);
                if (!has_displacement) {
                    return ErrorCode::E_ENG_UNSUPPORTED_EVENT_TYPE;
                }

                // TODO: set movement state
            }

            break;
        }
        default:
            return ErrorCode::E_ENG_UNSUPPORTED_EVENT_TYPE;
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode FirstPersonCameraOperator::RegisterEventHandlers() {
        auto forward_move = std::make_unique<MoveCommand>(
            m_name + ".MoveForward", CameraDirection::E_CAMERA_DIRECTION_FORWARD, *this
        );
        auto back_move = std::make_unique<MoveCommand>(
            m_name + ".MoveBack", CameraDirection::E_CAMERA_DIRECTION_BACK, *this
        );
        auto left_move = std::make_unique<MoveCommand>(
            m_name + ".MoveLeft", CameraDirection::E_CAMERA_DIRECTION_LEFT, *this
        );
        auto right_move = std::make_unique<MoveCommand>(
            m_name + ".MoveRight", CameraDirection::E_CAMERA_DIRECTION_RIGHT, *this
        );
        QCE_CRITICAL(ResourceManager::Get().AddCommand(std::move(forward_move)));
        QCE_CRITICAL(ResourceManager::Get().AddCommand(std::move(back_move)));
        QCE_CRITICAL(ResourceManager::Get().AddCommand(std::move(left_move)));
        QCE_CRITICAL(ResourceManager::Get().AddCommand(std::move(right_move)));

        return ErrorCode::SUCCESS;
    }
}
