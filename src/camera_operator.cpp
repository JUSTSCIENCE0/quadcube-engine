// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/camera_operator.hpp>
#include <qce/ancillary/timer.hpp>

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
                switch (m_direction) {
                case CameraDirection::E_CAMERA_DIRECTION_FORWARD:
                    if (is_down)
                        m_fpco.m_velocity.MoveForward(true);
                    else
                        m_fpco.m_velocity.StopForward();
                    break;
                case CameraDirection::E_CAMERA_DIRECTION_BACK:
                    if (is_down)
                        m_fpco.m_velocity.MoveForward(false);
                    else
                        m_fpco.m_velocity.StopForward();
                    break;
                case CameraDirection::E_CAMERA_DIRECTION_LEFT:
                    if (is_down)
                        m_fpco.m_velocity.MoveSide(true);
                    else
                        m_fpco.m_velocity.StopSide();
                    break;
                case CameraDirection::E_CAMERA_DIRECTION_RIGHT:
                    if (is_down)
                        m_fpco.m_velocity.MoveSide(false);
                    else
                        m_fpco.m_velocity.StopSide();
                    break;
                default:
                    assert(!"Unsupported direction");
                    break;
                }
            }
            else {
                bool has_displacement = hid_event_has_displacement(event_code);
                if (!has_displacement) {
                    return ErrorCode::E_ENG_UNSUPPORTED_EVENT_TYPE;
                }

                float3d velocity{
                    hid_event->param1,
                    0.0f,
                    hid_event->param2
                };
                m_fpco.m_velocity.SetVelocity(velocity);
                m_fpco.m_need_reset_velocity = true;
            }

            break;
        }
        default:
            return ErrorCode::E_ENG_UNSUPPORTED_EVENT_TYPE;
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode FirstPersonCameraOperator::RotateCommand::Execute(const CommandContext* context) {
        assert(context);
        switch (context->type) {
        case CommandContextType::E_CCT_HID_EVENT: {
            auto hid_event = static_cast<const HidEvent*>(context);
            auto event_code = hid_event->GetCode();
            auto is_button = hid_event_is_button(event_code);
            if (is_button) {
                // bool is_down = hid_event->IsButtonDown();
                switch (m_direction) {
                case CameraDirection::E_CAMERA_DIRECTION_UP:
                    // TODO
                    break;
                case CameraDirection::E_CAMERA_DIRECTION_DOWN:
                    // TODO
                    break;
                case CameraDirection::E_CAMERA_DIRECTION_LEFT:
                    // TODO
                    break;
                case CameraDirection::E_CAMERA_DIRECTION_RIGHT:
                    // TODO
                    break;
                default:
                    assert(!"Unsupported direction");
                    break;
                }
            }
            else {
                bool has_displacement = hid_event_has_displacement(event_code);
                if (!has_displacement) {
                    return ErrorCode::E_ENG_UNSUPPORTED_EVENT_TYPE;
                }
                
                // TODO: configurable sensitive for mouse and gamepad
                float sens = hid_event_is_gamepad(event_code) ? 0.001f : 1.0f;

                m_fpco.m_target_move = {
                    hid_event->param2 * sens,
                    hid_event->param1 * sens,
                    0.0f
                };
                m_fpco.m_need_reset_target_move = true;
            }

            break;
        }
        default:
            return ErrorCode::E_ENG_UNSUPPORTED_EVENT_TYPE;
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode FirstPersonCameraOperator::Update() {
        assert(m_camera);

        m_camera->Move(m_velocity, FrameTime::Get().Elapsed());
        if (m_need_reset_velocity) {
            m_velocity.SetDirection({ 0.0f, 0.0f, 0.0f });
            m_need_reset_velocity = false;
        }
        m_camera->RotateView(m_target_move.y(), m_target_move.x());
        if (m_need_reset_target_move) {
            m_target_move = {};
            m_need_reset_target_move = false;
        }
        return ErrorCode::SUCCESS;
    }

    ErrorCode FirstPersonCameraOperator::RegisterEventHandlers() {
#define REGISTRER_COMMAND(CommandType, Suffix, Direction) { \
            auto command_ptr = std::make_shared<CommandType>( \
                m_name + "." #Suffix, CameraDirection::E_CAMERA_DIRECTION_ ## Direction, *this); \
            auto id = command_ptr->m_name; \
            QCE_CRITICAL( \
                ResourceManager::Get().Add(Command{ \
                    std::move(id), \
                    std::move(command_ptr) \
                }) \
            ); \
        }

        REGISTRER_COMMAND(MoveCommand,   MoveForward, FORWARD)
        REGISTRER_COMMAND(MoveCommand,   MoveBack,    BACK)
        REGISTRER_COMMAND(MoveCommand,   MoveLeft,    LEFT)
        REGISTRER_COMMAND(MoveCommand,   MoveRight,   RIGHT)
        REGISTRER_COMMAND(RotateCommand, RotateUp,    UP)
        REGISTRER_COMMAND(RotateCommand, RotateDown,  DOWN)
        REGISTRER_COMMAND(RotateCommand, RotateLeft,  LEFT)
        REGISTRER_COMMAND(RotateCommand, RotateRight, RIGHT)

#undef REGISTRER_COMMAND

        return ErrorCode::SUCCESS;
    }
}
