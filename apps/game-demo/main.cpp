// Copyright (c) 2025, Yakov Usoltsev
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
            .caption = L"QCE Game Demo"
        }
    };

    auto& app = QCE::Application::Get(config);

    QCE::CuboidParams cuboid{
        .length = 1.0f,
        .width = 1.0f,
        .height = 1.0f
    };
    QCE_CRITICAL(app.Resources().AddShader("simple", QCE::ShaderType::E_VERTEX_SHADER));
    QCE_CRITICAL(app.Resources().AddShader("simple", QCE::ShaderType::E_PIXEL_SHADER));
    QCE_CRITICAL(app.Resources().AddFigure(cuboid, "cuboid"));
    QCE_CRITICAL(app.Resources().AddModel("cuboid", "cuboid"));

    QCE_CRITICAL(app.CurrentScene().AddCamera());

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
    QCE_CRITICAL(app.CurrentScene().UseShader("simple", QCE::ShaderType::E_VERTEX_SHADER));
    QCE_CRITICAL(app.CurrentScene().UseShader("simple", QCE::ShaderType::E_PIXEL_SHADER));

    QCE::HidEventsManager::Config hid_events_config{};
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('W'), "CameraOperator0.MoveForward"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('A'), "CameraOperator0.MoveLeft"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('S'), "CameraOperator0.MoveBack"));
    hid_events_config.emplace_back(
        std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode('D'), "CameraOperator0.MoveRight"));
    //hid_events_config.emplace_back(
    //    std::make_unique<QCE::HidSingleEvent>(QCE::HidEventCode::E_HEC_MOUSE_MOVE, "HidDescribe"));
    QCE_CRITICAL(app.HidManager().Setup(hid_events_config));

    return app.Run();
}