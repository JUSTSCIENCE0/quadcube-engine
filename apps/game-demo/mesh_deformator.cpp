// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include "mesh_deformator.hpp"

#include <qce/objects/resource_manager.hpp>

#include <stdexcept>
#include <cmath>
#include <cstring>

QCE::ErrorCode HillsAnimationSystem::UpdateScene() {
    auto entities = m_entities.QueryEntities<
        QCE::DynamicMesh,
        DeformationDescription>();

    for (const auto& entity_id : entities) {
        auto& mesh_component = m_entities.GetComponent<QCE::DynamicMesh>(entity_id);
        auto& base_mesh = QCE::ResourceManager::Get().Read<QCE::Mesh>(mesh_component.base_mesh_index);
        assert(base_mesh.indices.size() % 3 == 0);

        mesh_component.max_vertices_count = static_cast<uint32_t>(base_mesh.vertices.size());
        mesh_component.max_indeces_count  = static_cast<uint32_t>(base_mesh.indices.size());

        auto& deformated_mesh = QCE::ResourceManager::Get().Read<QCE::Mesh>(mesh_component.deformated_mesh_index);
        deformated_mesh.indices = base_mesh.indices;
        deformated_mesh.vertices = base_mesh.vertices;

        auto& deformation_component = m_entities.GetComponent<DeformationDescription>(entity_id);
        assert(deformation_component.update_period_sec > 0.0f);

        m_animation_cache_map.add(entity_id, m_animation_cache.size());
        const auto ncols = CalculateVerticiesCount(deformation_component.plane_params, false);
        const auto nrows = CalculateVerticiesCount(deformation_component.plane_params, true);
        const auto hcount = ncols * nrows;
        auto& entry = m_animation_cache.emplace_back(AnimationCache{
            .width = deformation_component.plane_params.width,
            .length = deformation_component.plane_params.length,
            .number_columns = ncols,
            .number_rows = nrows,
            .hills_count = hcount,
            .hills_count_rounded = hcount - (hcount % 16),
            .wstep = deformation_component.plane_params.width / static_cast<float>(ncols - 1),
            .lstep = deformation_component.plane_params.length / static_cast<float>(nrows - 1),
            .whalf = deformation_component.plane_params.width * 0.5f,
            .lhalf = deformation_component.plane_params.length * 0.5f,
            .poligons_count = base_mesh.indices.size() / 3,
            .is_reflected = deformation_component.is_reflected
        });

        CalculateBaseHeights(entry);
        CalculateNewHillsHeights(entry);
        UpdateMesh(entry, deformated_mesh);
    }

    return QCE::ErrorCode::SUCCESS;
}

QCE::ErrorCode HillsAnimationSystem::Update() {
    auto entities = m_entities.QueryEntities<
        QCE::DynamicMesh,
        DeformationDescription>();

    for (const auto& entity_id : entities) {
        assert(m_animation_cache_map.exists(entity_id));

        auto& deformation_component = m_entities.GetComponent<DeformationDescription>(entity_id);
        auto& mesh_component = m_entities.GetComponent<QCE::DynamicMesh>(entity_id);
        auto& deformated_mesh = QCE::ResourceManager::Get().Read<QCE::Mesh>(mesh_component.deformated_mesh_index);
        auto& entry = m_animation_cache[m_animation_cache_map[entity_id]];

        float elapsed_sec = 0.0f;
        if (entry.timer.Check(deformation_component.update_period_sec, elapsed_sec)) {
            CalculateNewHillsHeights(entry);
            entry.hills_hight = entry.hills_hight_prev;
        }
        else {
            auto t = elapsed_sec / deformation_component.update_period_sec;

            assert(t >= 0.0f && t <= 1.0f);

            size_t i = 0;
            auto prev = entry.hills_hight_prev.data();
            auto next = entry.hills_hight_next.data();
            auto out  = entry.hills_hight.data();
            for (; i < entry.hills_count_rounded;
                i += 16, prev += 16, next += 16, out += 16) {
                QCE::lerp16(prev, next, t, out);
            }
            for (; i < entry.hills_count; i++) {
                entry.hills_hight[i] = QCE::lerp(entry.hills_hight_prev[i], entry.hills_hight_next[i], t);
            }
        }

        UpdateMesh(entry, deformated_mesh);
    }

    return QCE::ErrorCode::SUCCESS;
}

size_t HillsAnimationSystem::CalculateVerticiesCount(const QCE::PlaneParams& plane_params, bool rows) {
    if (plane_params.unit_squares) {
        if (rows)
            return static_cast<size_t>(plane_params.length) + 1;
        else
            return static_cast<size_t>(plane_params.width) + 1;
    }
    else
        return 2;
}

void HillsAnimationSystem::CalculateBaseHeights(AnimationCache& out) {
    out.hills_base.reserve(out.hills_count);

    const auto start_x = out.is_reflected ? out.whalf : -out.whalf;
    const auto step_x = out.is_reflected ? -out.wstep : out.wstep;
    auto is_finished_x = [&](float x) {
        if (out.is_reflected)
            return x < -out.whalf;
        else
            return x > out.whalf;
        };

    int xi = 0;
    int zi = 0;

    for (float z = out.lhalf;
        z >= -out.lhalf;
        z -= out.lstep) {
        for (float x = start_x;
            !is_finished_x(x);
            x += step_x) {
            auto y = std::sqrtf(x + out.whalf) * (std::sqrtf(0.05f * (z + out.lhalf)) + 0.5f);
            if (xi % 2 == 0 || zi % 2 == 0) {
                y *= 0.75f;
            }

            out.hills_base.push_back(y);

            xi++;
        }

        zi++;
    }

    assert(out.hills_base.size() == out.hills_count);

    out.hills_hight.resize(out.hills_count);
    out.hills_hight_next = out.hills_base;
}

void HillsAnimationSystem::CalculateNewHillsHeights(AnimationCache& entry) {
    entry.hills_hight_prev.swap(entry.hills_hight_next);

    assert(entry.hills_base.size() == entry.hills_count);
    entry.hills_hight_next = entry.hills_base;

    for (auto& height : entry.hills_hight_next)
        height *= m_rnd.get();
}

void HillsAnimationSystem::UpdateMesh(AnimationCache& entry, QCE::Mesh& mesh) {
    for (auto& vertex : mesh.vertices) {
        const auto x = vertex.position.x();
        const auto z = vertex.position.z();
        auto& y = vertex.position.y();

        // calculate index in hills cache
        const auto xi = static_cast<size_t>((entry.whalf + x) / entry.wstep);
        const auto zi = static_cast<size_t>((entry.lhalf - z) / entry.lstep);
        assert(xi < entry.number_columns && zi < entry.number_rows);

        const auto index = xi + zi * entry.number_columns;
        assert(index < entry.hills_hight.size());
        y = entry.hills_hight[index];
    }

    assert(mesh.indices.size() % 3 == 0);
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        auto i0 = mesh.indices[i];
        auto i1 = mesh.indices[i + 1];
        auto i2 = mesh.indices[i + 2];
        auto& v0 = mesh.vertices[i0];
        auto& v1 = mesh.vertices[i1];
        auto& v2 = mesh.vertices[i2];

        v0.normal = QCE::compute_normal(v0, v1, v2);
        v1.normal = v0.normal;
        v2.normal = v0.normal;
    }
}

