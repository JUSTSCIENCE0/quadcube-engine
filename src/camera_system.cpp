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
            const std::string& camera_name,
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

        if (camera_type != E_CAMERA_FIXED) {
            Movement movement{
                .linear_speed = 1.0f // TODO: make configurable
            };
            QCE_CRITICAL(m_entities.AddComponent(camera_entity_id, movement));
        }

        EntityName entity_name{
            .name = camera_name
        };
        QCE_CRITICAL(m_entities.AddComponent(camera_entity_id, entity_name));

        TransformComponents transform{
            .position = position
        };
        QCE_CRITICAL(m_entities.AddComponent(camera_entity_id, transform));

        CameraView camera_view{
            .is_LH = is_LH_system,

            .position = position,
            .target = target,
            .up_direction = up
        };
        QCE_CRITICAL(m_entities.AddComponent(camera_entity_id, camera_view));

        CameraProj camera_proj{
            .is_LH = is_LH_system,

            .fov_rad = fov_rad,
            .aspect = aspect,
            .znear = znear,
            .zfar = zfar
        };
        QCE_CRITICAL(m_entities.AddComponent(camera_entity_id, camera_proj));

        if (camera_type == E_CAMERA_FIRST_PERSON) {
            QCE_CRITICAL(m_entities.AddComponent(camera_entity_id, FirstPersonCameraControl{}));
            QCE_CRITICAL(RegisterFpcEventHandlers(camera_name, camera_entity_id));
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode CameraSystem::Update() {
        auto cameras = m_entities.QueryEntities<CameraView>();
        for (const auto& camera_id : cameras) {
            if (m_entities.HasComponent<FirstPersonCameraControl>(camera_id)) {
                auto& camera_fp_control = m_entities.GetComponent<FirstPersonCameraControl>(camera_id);
                auto& camera_view = m_entities.GetComponent<CameraView>(camera_id);
                camera_rotate_view(
                    camera_view,
                    camera_fp_control.view_rotation.y(),
                    camera_fp_control.view_rotation.x()
                );

                if (camera_fp_control.reset_view_rotation) {
                    camera_fp_control.view_rotation = {};
                    camera_fp_control.reset_view_rotation = false;
                }
            }
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode CameraSystem::RegisterFpcEventHandlers(std::string camera_name, entity_id_t camera_id) {
#define REGISTRER_COMMAND(CommandType, Suffix, Direction) { \
            auto command_ptr = std::make_shared<CommandType>( \
                camera_name + "." #Suffix, m_entities, camera_id, CameraDirection::E_CAMERA_DIRECTION_ ## Direction); \
            auto id = command_ptr->m_name; \
            QCE_CRITICAL( \
                ResourceManager::Get().Add(Command{ \
                    std::move(id), \
                    std::move(command_ptr) \
                }) \
            ); \
        }

            REGISTRER_COMMAND(MoveCommand, MoveForward, FORWARD)
            REGISTRER_COMMAND(MoveCommand, MoveBack, BACK)
            REGISTRER_COMMAND(MoveCommand, MoveLeft, LEFT)
            REGISTRER_COMMAND(MoveCommand, MoveRight, RIGHT)
            REGISTRER_COMMAND(RotateCommand, RotateUp, UP)
            REGISTRER_COMMAND(RotateCommand, RotateDown, DOWN)
            REGISTRER_COMMAND(RotateCommand, RotateLeft, LEFT)
            REGISTRER_COMMAND(RotateCommand, RotateRight, RIGHT)

#undef REGISTRER_COMMAND

        return ErrorCode::SUCCESS;
    }

    ErrorCode CameraSystem::MoveCommand::Execute(const CommandContext* context) {
        assert(context);

        auto& move = m_entities.GetComponent<Movement>(m_camera_id);

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
                        movement_apply_forward(move, true);
                    else
                        movement_stop_forward(move);
                    break;
                case CameraDirection::E_CAMERA_DIRECTION_BACK:
                    if (is_down)
                        movement_apply_forward(move, false);
                    else
                        movement_stop_forward(move);
                    break;
                case CameraDirection::E_CAMERA_DIRECTION_LEFT:
                    if (is_down)
                        movement_apply_side(move, true);
                    else
                        movement_stop_side(move);
                    break;
                case CameraDirection::E_CAMERA_DIRECTION_RIGHT:
                    if (is_down)
                        movement_apply_side(move, false);
                    else
                        movement_stop_side(move);
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
                movement_calc_from_velocity(move, velocity);
                move.need_reset_direction = true;
            }

            break;
        }
        default:
            return ErrorCode::E_ENG_UNSUPPORTED_EVENT_TYPE;
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode CameraSystem::RotateCommand::Execute(const CommandContext* context) {
        assert(context);

        auto& camera = m_entities.GetComponent<FirstPersonCameraControl>(m_camera_id);

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

                camera.view_rotation = {
                    hid_event->param2 * sens,
                    hid_event->param1 * sens,
                    0.0f
                };
                camera.reset_view_rotation = true;
            }

            break;
        }
        default:
            return ErrorCode::E_ENG_UNSUPPORTED_EVENT_TYPE;
        }

        return ErrorCode::SUCCESS;
    }
}
