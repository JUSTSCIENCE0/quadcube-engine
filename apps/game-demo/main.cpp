// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include "mesh_deformator.hpp"

#include <qce/qce.hpp>
#include <qce/ancillary/directories.hpp>

int main(int argc, char* argv[]) {
#ifdef NDEBUG
    FreeConsole();
#endif

    auto& app = QCE::Application<
        HillsAnimationSystem
    >::Get();
    QCE_CRITICAL(
    app.Setup<
        HillsAnimationConfig
    >());

    QCE_CRITICAL(app.LoadScene("game_demo"));

    // Custom components
    // TODO: add it to scene description and load automatically

    QCE::PlaneParams hills{
        .length = 100.0f,
        .width = 20.0f,
        .hard_edges = true,
        .repeat_uv = true,
        .unit_squares = true
    };
    DeformationDescription hills_deformation_right{
        .update_period_sec = 0.5f,
        .plane_params = hills,
        .is_reflected = false
    };
    DeformationDescription hills_deformation_left{
        .update_period_sec = 0.5f,
        .plane_params = hills,
        .is_reflected = true
    };
    for (const auto entity_id : app.m_entities.QueryEntities<QCE::EntityName, QCE::DynamicMesh>()) {
        const auto& entity_name = app.m_entities.GetComponent<QCE::EntityName>(entity_id).name;
        if (entity_name == "right_hills") {
            QCE_CRITICAL(app.m_entities.AddComponent(entity_id, hills_deformation_right));
        }
        else if (entity_name == "left_hills") {
            QCE_CRITICAL(app.m_entities.AddComponent(entity_id, hills_deformation_left));
        }
    }

    QCE_CRITICAL(
        app.m_systems.Get<HillsAnimationSystem>().UpdateScene());
    return app.Run();
}