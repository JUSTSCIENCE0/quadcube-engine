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

    QCE::CuboidParams cuboid{
        .length = 1.0f,
        .width = 1.0f,
        .height = 1.0f
    };
    QCE_CRITICAL(app.Resources().AddFigure(cuboid, "cuboid"));

    QCE::SphereParams sphere_smooth{
        .radius = 0.5f,
        .subdivisions = 2,
        .hard_edges = false
    };
    QCE_CRITICAL(app.Resources().AddFigure(sphere_smooth, "sphere_smooth"));

    QCE::SphereParams sphere_hard{
        .radius = 0.5f,
        .subdivisions = 2,
        .hard_edges = true
    };
    QCE_CRITICAL(app.Resources().AddFigure(sphere_hard, "sphere_hard"));

    QCE::PlaneParams flat_plane{
        .length = 50.0f,
        .width = 5.0f,
        .hard_edges = false,
        .repeat_uv = true,
        .unit_squares = true
    };
    QCE_CRITICAL(app.Resources().AddFigure(flat_plane, "flat_plane"));

    QCE::PlaneParams hills{
        .length = 100.0f,
        .width = 20.0f,
        .hard_edges = true,
        .repeat_uv = true,
        .unit_squares = true
    };
    QCE_CRITICAL(app.Resources().AddFigure(hills, "hills"));

    QCE::Mesh left_hills_mesh;
    left_hills_mesh.id = "left_hills";
    QCE_CRITICAL(app.Resources().Add(std::move(left_hills_mesh)));

    QCE::Mesh right_hills_mesh;
    right_hills_mesh.id = "right_hills";
    QCE_CRITICAL(app.Resources().Add(std::move(right_hills_mesh)));

    QCE_CRITICAL(app.Resources().AddTexture("squares.bc7"));
    QCE::Material textured_material{};
    textured_material.id = "textured_material";
    textured_material.albedo_color = QCE::LIGHT_GRAY;
    textured_material.albedo_texture = app.Resources().GetIndex<QCE::Texture2D>("squares.bc7");
    QCE_CRITICAL(app.Resources().Add(std::move(textured_material)));

    QCE::Material untextured_material{};
    untextured_material.id = "untextured_material";
    untextured_material.albedo_color = QCE::GRAY;
    QCE_CRITICAL(app.Resources().Add(std::move(untextured_material)));

    QCE_CRITICAL(app.Resources().AddTexture("edges.bc7"));
    QCE::Material edges_material{};
    edges_material.id = "edges_material";
    edges_material.albedo_color = QCE::WHITE;
    edges_material.albedo_texture = app.Resources().GetIndex<QCE::Texture2D>("edges.bc7");
    QCE_CRITICAL(app.Resources().Add(std::move(edges_material)));

    QCE::TransformAnimation square_path{};
    square_path.id = "square_path";
    square_path.position_channel = {
        {
            /*value*/ { 1.0f, 0.0f, 1.0f },
            /*start_time*/ 0.0f
        },
        {
            /*value*/ { 1.0f, 0.0f, -1.0f },
            /*start_time*/ 1.0f
        },
        {
            /*value*/ { 1.0f, 1.0f, -1.0f },
            /*start_time*/ 2.0f
        },
        {
            /*value*/ { 1.0f, 1.0f, 1.0f },
            /*start_time*/ 3.0f
        },
        {
            /*value*/ { 1.0f, 0.0f, 1.0f },
            /*start_time*/ 4.0f
        }
    };
    square_path.scale_channel = {
        {
            /*value*/ { 1.0f, 1.0f, 1.0f },
            /*start_time*/ 1.0f
        },
        {
            /*value*/ { 1.0f, 3.0f, 1.0f },
            /*start_time*/ 2.0f
        },
        {
            /*value*/ { 3.0f, 1.0f, 1.0f },
            /*start_time*/ 3.0f
        },
        {
            /*value*/ { 1.0f, 1.0f, 1.0f },
            /*start_time*/ 4.0f
        }
    };
    square_path.total_duration = QCE::calculate_animation_duration(square_path);
    QCE_CRITICAL(QCE::validate_animation(square_path));
    QCE_CRITICAL(app.Resources().Add(std::move(square_path)));

    QCE::TransformAnimation sphere_rotation{};
    sphere_rotation.id = "sphere_rotation";
    sphere_rotation.rotation_channel = {
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 0.0f, 0.0f),
            /*start_time*/ 0.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 90.0f, 0.0f),
            /*start_time*/ 1.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 180.0f, 0.0f),
            /*start_time*/ 2.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 270.0f, 0.0f),
            /*start_time*/ 3.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 0.0f, 0.0f),
            /*start_time*/ 4.0f
        }
    };
    sphere_rotation.total_duration = QCE::calculate_animation_duration(sphere_rotation);
    QCE_CRITICAL(QCE::validate_animation(sphere_rotation));
    QCE_CRITICAL(app.Resources().Add(std::move(sphere_rotation)));

    QCE::StaticMesh cuboid_mesh_component{
        .index = app.Resources().GetIndex<QCE::Mesh>("cuboid")
    };
    QCE::StaticMesh peaks_plane_mesh_component_left{
        .index = app.Resources().GetIndex<QCE::Mesh>("hills")
    };
    QCE::DynamicMesh hills_mesh_component_right{
        .base_mesh_index = app.Resources().GetIndex<QCE::Mesh>("hills"),
        .deformated_mesh_index = app.Resources().GetIndex<QCE::Mesh>("right_hills")
    };
    DeformationDescription hills_deformation_right{
        .update_period_sec = 0.5f,
        .plane_params = hills,
        .is_reflected = false
    };
    QCE::DynamicMesh hills_mesh_component_left{
        .base_mesh_index = app.Resources().GetIndex<QCE::Mesh>("hills"),
        .deformated_mesh_index = app.Resources().GetIndex<QCE::Mesh>("left_hills")
    };
    DeformationDescription hills_deformation_left{
        .update_period_sec = 0.5f,
        .plane_params = hills,
        .is_reflected = true
    };
    QCE::MaterialComponent textured_material_component{
        .index = app.Resources().GetIndex<QCE::Material>("textured_material")
    };
    QCE::MaterialComponent untextured_material_component{
        .index = app.Resources().GetIndex<QCE::Material>("untextured_material")
    };
    QCE::MaterialComponent edges_material_component{
        .index = app.Resources().GetIndex<QCE::Material>("edges_material")
    };
    QCE::TransformAnimationComponent square_path_component{
        .index = app.Resources().GetIndex<QCE::TransformAnimation>("square_path"),
        .is_looped = true
    };
    QCE::TransformAnimationComponent sphere_rotation_component{
        .index = app.Resources().GetIndex<QCE::TransformAnimation>("sphere_rotation"),
         .is_looped = true
    };

    auto entity0 = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity0, cuboid_mesh_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity0,
        QCE::TransformComponents{
            //{ 0.0f, 0.3826834f, 0.0f, 0.9238795f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity0, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity0, edges_material_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity0, square_path_component));

    auto entity1 = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity1, cuboid_mesh_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity1,
        QCE::TransformComponents{
            //{ 0.0f, 0.3826834f, 0.0f, 0.9238795f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { -1.f, 0.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity1, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity1, textured_material_component));

    auto entity2 = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity2, QCE::StaticMesh{
        .index = app.Resources().GetIndex<QCE::Mesh>("sphere_smooth")
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity2,
        QCE::TransformComponents{
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity2, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity2, untextured_material_component));

    auto entity3 = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity3, QCE::StaticMesh{
        .index = app.Resources().GetIndex<QCE::Mesh>("sphere_hard")
    }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity3,
        QCE::TransformComponents{
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 0.0f, 3.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity3, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity3, untextured_material_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity3, sphere_rotation_component));

    auto entity4 = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity4, QCE::StaticMesh{
        .index = app.Resources().GetIndex<QCE::Mesh>("flat_plane")
    }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity4,
        QCE::TransformComponents{
            //{ 0.0f, 0.3826834f, 0.0f, 0.9238795f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 0.0f, -.5f, 20.5f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity4, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity4, edges_material_component));

    auto entity_id = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, hills_mesh_component_right));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, hills_deformation_right));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id,
        QCE::TransformComponents{
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 7.5f, -.5f, 20.5f },
            { 0.5f, 0.5f, 0.5f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, edges_material_component));

    entity_id = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, hills_mesh_component_left));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, hills_deformation_left));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id,
        QCE::TransformComponents{
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { -7.5f, -.5f, 20.5f },
            { 0.5f, 0.5f, 0.5f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, edges_material_component));

    auto sun = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(sun,
        QCE::DirectionalLight{
            .color = { 0.5f, 0.5f, 0.5f, 1.0f },
            .direction = { 1.0f, -2.0f, 3.0f }
        }
    ));

    QCE_CRITICAL(
        app.m_systems.Get<HillsAnimationSystem>().UpdateScene());
    return app.Run();
}