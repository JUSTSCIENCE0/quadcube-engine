// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD


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

    QCE::SphereParams sphere_hard{
        .radius = 0.5f,
        .subdivisions = 2,
        .hard_edges = true
    };
    QCE_CRITICAL(app.Resources().AddFigure(sphere_hard, "sphere_hard"));

    QCE::PlaneParams flat_plane{
        .length = 50.0f,
        .width = 50.0f,
        .hard_edges = false,
        .repeat_uv = true,
        .unit_squares = true
    };
    QCE_CRITICAL(app.Resources().AddFigure(flat_plane, "flat_plane"));

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

    QCE::TransformAnimation deformation_demo{};
    deformation_demo.id = "deformation_demo";
    deformation_demo.position_channel = {
        {
            /*value*/ { 1.5f, 0.0f, 1.5f },
            /*start_time*/ 0.0f
        },
        {
            /*value*/ { 1.5f, 0.0f, -1.5f },
            /*start_time*/ 1.0f
        },
        {
            /*value*/ { 1.5f, 1.0f, -1.5f },
            /*start_time*/ 2.0f
        },
        {
            /*value*/ { 1.5f, 1.0f, 1.5f },
            /*start_time*/ 3.0f
        },
        {
            /*value*/ { 1.5f, 0.0f, 1.5f },
            /*start_time*/ 4.0f
        }
    };
    deformation_demo.scale_channel = {
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
    deformation_demo.total_duration = QCE::calculate_animation_duration(deformation_demo);
    QCE_CRITICAL(QCE::validate_animation(deformation_demo));
    QCE_CRITICAL(app.Resources().Add(std::move(deformation_demo)));

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
    QCE::StaticMesh sphere_mesh_component{
        .index = app.Resources().GetIndex<QCE::Mesh>("sphere_hard")
    };
    QCE::StaticMesh plane_mesh_component{
        .index = app.Resources().GetIndex<QCE::Mesh>("flat_plane")
    };

    QCE::MaterialComponent untextured_material_component{
        .index = app.Resources().GetIndex<QCE::Material>("untextured_material")
    };
    QCE::MaterialComponent edges_material_component{
        .index = app.Resources().GetIndex<QCE::Material>("edges_material")
    };
    QCE::TransformAnimationComponent deformation_demo_component{
        .index = app.Resources().GetIndex<QCE::TransformAnimation>("deformation_demo"),
        .is_looped = true
    };
    QCE::TransformAnimationComponent sphere_rotation_component{
        .index = app.Resources().GetIndex<QCE::TransformAnimation>("sphere_rotation"),
         .is_looped = true
    };

    // floor plane
    auto entity_id = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, plane_mesh_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id,
        QCE::TransformComponents{
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 0.0f, -.5f, 0.0f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, edges_material_component));

    // sphere
    entity_id = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, sphere_mesh_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id,
        QCE::TransformComponents{
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f }
        }));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, untextured_material_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, sphere_rotation_component));

    // cuboid deformation
    entity_id = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, cuboid_mesh_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformComponents{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformMatrix{}));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, edges_material_component));
    QCE_CRITICAL(app.m_entities.AddComponent(entity_id, deformation_demo_component));

    // spline demo
    float z_coord = 3.5f;
    auto add_spline_demo_animation= [&app, &z_coord, &cuboid_mesh_component, &untextured_material_component](
            QCE::SplineFunc spline_func) {
        std::string name = QCE::to_string(spline_func);

        QCE::TransformAnimation animation{};
        animation.id = name;
        animation.position_channel = {
            {
                /*value*/ { -1.5f, 0.0f, z_coord },
                /*start_time*/ 0.0f
            },
            {
                /*value*/ { -2.5f, 0.0f, z_coord + 1.0f },
                /*start_time*/ 1.0f
            },
            {
                /*value*/ { -1.5f, 0.0f, z_coord + 2.0f },
                /*start_time*/ 2.0f
            },
            {
                /*value*/ { 1.5f, 0.0f, z_coord },
                /*start_time*/ 3.0f
            },
            {
                /*value*/ { 2.5f, 0.0f, z_coord + 1.0f },
                /*start_time*/ 4.0f
            },
            {
                /*value*/ { 1.5f, 0.0f, z_coord + 2.0f },
                /*start_time*/ 5.0f
            },
            {
                /*value*/ { -1.5f, 0.0f, z_coord },
                /*start_time*/ 6.0f
            }
        };
        animation.spline_func = spline_func;
        animation.total_duration = QCE::calculate_animation_duration(animation);
        QCE_CRITICAL(QCE::validate_animation(animation));
        QCE_CRITICAL(app.Resources().Add(std::move(animation)));

        z_coord += 3.0f;

        auto entity_id = app.m_entities.AddEntity();
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, cuboid_mesh_component));
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformComponents{}));
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformMatrix{}));
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, untextured_material_component));
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformAnimationComponent{
            .index = app.Resources().GetIndex<QCE::TransformAnimation>(name),
            .is_looped = true
        }));

        return QCE::ErrorCode::SUCCESS;
    };
    QCE_CRITICAL(add_spline_demo_animation(QCE::SplineFunc::E_SPLINE_LINEAR));
    QCE_CRITICAL(add_spline_demo_animation(QCE::SplineFunc::E_SPLINE_CATMULL_ROM));

    // easing demos
    float x_coord = -1.5f;
    auto add_easing_demo_animation = [&app, &x_coord, &cuboid_mesh_component, &untextured_material_component](
            QCE::EasingFunc easing_func) {
        std::string name = QCE::to_string(easing_func);

        QCE::TransformAnimation animation{};
        animation.id = name;
        animation.position_channel = {
            {
                /*value*/ { x_coord, 0.0f, 2.5f },
                /*start_time*/ 0.0f,
                /*easing_func*/ easing_func
            },
            {
                /*value*/ { x_coord, 0.0f, -2.5f },
                /*start_time*/ 1.0f,
                /*easing_func*/ easing_func
            },
            {
                /*value*/ { x_coord, 0.0f, 2.5f },
                /*start_time*/ 2.0f,
                /*easing_func*/ easing_func
            }
        };
        animation.total_duration = QCE::calculate_animation_duration(animation);
        QCE_CRITICAL(QCE::validate_animation(animation));
        QCE_CRITICAL(app.Resources().Add(std::move(animation)));

        x_coord -= 1.0f;

        auto entity_id = app.m_entities.AddEntity();
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, cuboid_mesh_component));
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformComponents{}));
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformMatrix{}));
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, untextured_material_component));
        QCE_CRITICAL(app.m_entities.AddComponent(entity_id, QCE::TransformAnimationComponent{
            .index = app.Resources().GetIndex<QCE::TransformAnimation>(name),
            .is_looped = true
        }));

        return QCE::ErrorCode::SUCCESS;
    };
    QCE_CRITICAL(add_easing_demo_animation(QCE::EasingFunc::E_EASING_LINEAR));
    QCE_CRITICAL(add_easing_demo_animation(QCE::EasingFunc::E_EASING_EASE_IN_QUAD));
    QCE_CRITICAL(add_easing_demo_animation(QCE::EasingFunc::E_EASING_EASE_IN_EXPO));
    QCE_CRITICAL(add_easing_demo_animation(QCE::EasingFunc::E_EASING_EASE_OUT_QUAD));
    QCE_CRITICAL(add_easing_demo_animation(QCE::EasingFunc::E_EASING_EASE_OUT_SQRT));
    QCE_CRITICAL(add_easing_demo_animation(QCE::EasingFunc::E_EASING_SMOOTH_STEP));
    QCE_CRITICAL(add_easing_demo_animation(QCE::EasingFunc::E_EASING_SMOOTHER_STEP));

    // light
    auto sun = app.m_entities.AddEntity();
    QCE_CRITICAL(app.m_entities.AddComponent(sun,
        QCE::DirectionalLight{
            .color = { 0.5f, 0.5f, 0.5f, 1.0f },
            .direction = { 1.0f, -2.0f, 3.0f }
        }
    ));

    return app.Run();
}