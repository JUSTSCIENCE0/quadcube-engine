// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/qce.hpp>

static const std::string HID_CONFIG = R"(
{
    "event_descriptors": [
        {
            "type": "single",
            "handler": "MainCamera.MoveForward",
            "hid_event_code_name": "KEYBOARD_W"
        },
        {
            "type": "single",
            "handler": "MainCamera.MoveLeft",
            "hid_event_code_name": "KEYBOARD_A"
        },
        {
            "type": "single",
            "handler": "MainCamera.MoveBack",
            "hid_event_code_name": "KEYBOARD_S"
        },
        {
            "type": "single",
            "handler": "MainCamera.MoveRight",
            "hid_event_code_name": "KEYBOARD_D"
        },
        {
            "type": "single",
            "handler": "MainCamera.MoveForward",
            "hid_event_code_name": "KEYBOARD_UP"
        },
        {
            "type": "single",
            "handler": "MainCamera.MoveLeft",
            "hid_event_code_name": "KEYBOARD_LEFT"
        },
        {
            "type": "single",
            "handler": "MainCamera.MoveBack",
            "hid_event_code_name": "KEYBOARD_DOWN"
        },
        {
            "type": "single",
            "handler": "MainCamera.MoveRight",
            "hid_event_code_name": "KEYBOARD_RIGHT"
        },
        {
           "type": "single",
           "handler": "MainCamera.RotateUp",
           "hid_event_code_name": "MOUSE_MOVE"
        },
        {
            "type": "single",
            "handler": "MainCamera.MoveForward",
            "hid_event_code_name": "GAMEPAD_LSTICK_MOVE"
        },
        {
           "type": "single",
           "handler": "MainCamera.RotateUp",
           "hid_event_code_name": "GAMEPAD_RSTICK_MOVE"
        },
        {
            "type": "single",
            "handler": "Exit",
            "hid_event_code_name": "KEYBOARD_ESC"
        }
    ]
})";

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    FreeConsole();
#endif

    auto& app = QCE::Application<>::Get();

    QCE::ApplicationConfig<> config{
        .graphics_output {
            .caption = L"QCE Game Demo",
            .is_first_person = true
        }
    };
    auto& camera_config = std::get<QCE::CameraConfig>(config.systems_configs);
    camera_config = QCE::DEFAULT_CAMERA_CONFIG;

    auto& hid_events_config = std::get<QCE::HidConfig>(config.systems_configs);
    std::string error_descr = "";
    auto parse_result = macrojson::json_str_to_object(HID_CONFIG, hid_events_config, error_descr);
    if (macrojson::E_MJSON_OK != parse_result) {
        std::cerr << "HID Config parsing failed with error: " << error_descr << std::endl;
        return -1;
    }

    auto& render_config = std::get<QCE::RenderConfig>(config.systems_configs);
    render_config = QCE::DEFAULT_RENDER_CONFIG;

    QCE_CRITICAL(app.Setup(config));

    QCE::CuboidParams cuboid{
        .length = 1.0f,
        .width = 1.0f,
        .height = 1.0f
    };
    QCE_CRITICAL(app.Resources().AddFigure(cuboid, "cuboid"));

    QCE::MeshComponent cuboid_mesh_component{
        .index = app.Resources().GetIndex<QCE::Mesh>("cuboid")
    };
    auto entity0 = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity0, cuboid_mesh_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity0,
        QCE::TransformComponents{
            { 0.0f, 0.3826834f, 0.0f, 0.9238795f },
            { 2.0f, 0.0f, 2.0f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity0, QCE::TransformMatrix{}));

    auto entity1 = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity1, cuboid_mesh_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity1,
        QCE::TransformComponents{
            { 0.0f, 0.3826834f, 0.0f, 0.9238795f },
            { -2.0f, 0.0f, -2.0f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity1, QCE::TransformMatrix{}));

    return app.Run();
}