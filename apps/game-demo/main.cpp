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
        .length = 2.0f,
        .width = 1.0f,
        .height = 3.0f
    };
    QCE_CRITICAL(app.Resources().AddShader("simple", QCE::ShaderType::E_VERTEX_SHADER));
    QCE_CRITICAL(app.Resources().AddShader("simple", QCE::ShaderType::E_PIXEL_SHADER));
    QCE_CRITICAL(app.Resources().AddFigure(cuboid, "cuboid"));
    QCE_CRITICAL(app.Resources().AddModel("cuboid", "cuboid"));

    QCE_CRITICAL(app.CurrentScene().AddEntity("cuboid", "cuboid"));
    QCE_CRITICAL(app.CurrentScene().UseShader("simple", QCE::ShaderType::E_VERTEX_SHADER));
    QCE_CRITICAL(app.CurrentScene().UseShader("simple", QCE::ShaderType::E_PIXEL_SHADER));

    return app.Run();
}