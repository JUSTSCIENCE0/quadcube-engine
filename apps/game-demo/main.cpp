// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/qce.hpp>

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    FreeConsole();
#endif

    QCE::ApplicationConfig config{
        .graphics_output {
            .caption = L"QCE Game Demo",
            .is_first_person = true
        }
    };

    auto& app = QCE::Application<>::Get(config);

    QCE::CuboidParams cuboid{
        .length = 1.0f,
        .width = 1.0f,
        .height = 1.0f
    };
    QCE_CRITICAL(app.Resources().AddShader("simple", QCE::ShaderType::E_VERTEX_SHADER));
    QCE_CRITICAL(app.Resources().AddShader("simple", QCE::ShaderType::E_PIXEL_SHADER));
    QCE_CRITICAL(app.Resources().AddFigure(cuboid, "cuboid"));

    auto& cameras = app.m_systems.Get<QCE::CameraSystem>();
    QCE_CRITICAL(cameras.Setup(config.render.render_type));
    QCE_CRITICAL(cameras.AddCamera());

    QCE_CRITICAL(app.CurrentScene().AddEntity(
        "cuboid_1",
        "cuboid",
        QCE::Transform(
            { 0.0f, 0.3826834f, 0.0f, 0.9238795f },
            {2.0f, 0.0f, 2.0f},
            {1.0f, 1.0f, 1.0f}
        )));
    QCE_CRITICAL(app.CurrentScene().AddEntity(
        "cuboid_2",
        "cuboid",
        QCE::Transform(
            { 0.0f, 0.3826834f, 0.0f, 0.9238795f },
            { -2.0f, 0.0f, -2.0f },
            { 1.0f, 1.0f, 1.0f }
        )));
    QCE_CRITICAL(app.m_render->UseShader("simple", QCE::ShaderType::E_VERTEX_SHADER));
    QCE_CRITICAL(app.m_render->UseShader("simple", QCE::ShaderType::E_PIXEL_SHADER));

    QCE::HidEventsManager::Config hid_events_config{};
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('W'), "MainCamera.MoveForward"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('A'), "MainCamera.MoveLeft"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('S'), "MainCamera.MoveBack"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('D'), "MainCamera.MoveRight"));

    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_UP, "MainCamera.MoveForward"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_LEFT, "MainCamera.MoveLeft"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_DOWN, "MainCamera.MoveBack"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_RIGHT, "MainCamera.MoveRight"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_GAMEPAD_LSTICK_MOVE, "MainCamera.MoveForward"));

    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_MOUSE_MOVE, "MainCamera.RotateUp"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_GAMEPAD_RSTICK_MOVE, "MainCamera.RotateUp"));

    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_KEYBOARD_ESC, "Exit"));

    QCE_CRITICAL(app.HidManager().Setup(hid_events_config));

    return app.Run();
}