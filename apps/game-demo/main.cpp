// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/qce.hpp>

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
    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('W'), "MainCamera.MoveForward"));
    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('A'), "MainCamera.MoveLeft"));
    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('S'), "MainCamera.MoveBack"));
    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('D'), "MainCamera.MoveRight"));

    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_UP, "MainCamera.MoveForward"));
    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_LEFT, "MainCamera.MoveLeft"));
    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_DOWN, "MainCamera.MoveBack"));
    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_RIGHT, "MainCamera.MoveRight"));
    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_GAMEPAD_LSTICK_MOVE, "MainCamera.MoveForward"));

    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_MOUSE_MOVE, "MainCamera.RotateUp"));
    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_GAMEPAD_RSTICK_MOVE, "MainCamera.RotateUp"));

    hid_events_config.event_descriptors.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_ESC, "Exit"));

    auto& render_config = std::get<QCE::RenderConfig>(config.systems_configs);
    render_config.used_shaders = {
        { "simple", QCE::ShaderType::E_VERTEX_SHADER },
        { "simple", QCE::ShaderType::E_PIXEL_SHADER }
    };

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