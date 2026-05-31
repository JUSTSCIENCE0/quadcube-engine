// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/qce.hpp>
#include <qce/ancillary/directories.hpp>

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    FreeConsole();
#endif

    auto& app = QCE::Application<>::Get();
    QCE_CRITICAL(app.Setup<>());

    QCE::CuboidParams cuboid{
        .length = 1.0f,
        .width = 1.0f,
        .height = 1.0f
    };
    QCE_CRITICAL(app.Resources().AddFigure(cuboid, "cuboid"));
    QCE_CRITICAL(app.Resources().AddTexture("squares.bc7"));
    QCE::Material material{};
    material.id = "simple_material";
    material.albedo_color = QCE::GRAY;
    material.cpu_albedo_index = app.Resources().GetIndex<QCE::Texture2D>("squares.bc7");
    QCE_CRITICAL(app.Resources().Add(std::move(material)));

    QCE::MeshComponent cuboid_mesh_component{
        .index = app.Resources().GetIndex<QCE::Mesh>("cuboid")
    };
    QCE::MaterialComponent material_component{
        .index = app.Resources().GetIndex<QCE::Material>("simple_material")
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
    QCE_CRITICAL(app.m_entities.AddComponent(entity0, material_component));

    auto entity1 = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity1, cuboid_mesh_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity1,
        QCE::TransformComponents{
            { 0.0f, 0.3826834f, 0.0f, 0.9238795f },
            { -2.0f, 0.0f, -2.0f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity1, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity1, material_component));

    auto sun = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(sun,
        QCE::DirectionalLight{
            .color = { 0.5f, 0.5f, 0.5f, 1.0f },
            .direction = { 1.0f, -2.0f, 3.0f }
        }
    ));

    return app.Run();
}