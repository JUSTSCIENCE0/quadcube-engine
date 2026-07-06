// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include "mesh_deformator.hpp"

#include <stdexcept>
#include <cmath>
#include <cstring>

AnimateHills::AnimateHills(
    float update_period_sec,
    const QCE::PlaneParams& plane_params,
    const QCE::Mesh& base_mesh,
    bool is_reflected) :
        QCE::BaseCommand("AnimateHills"),
        m_base_mesh(base_mesh),
        m_width(plane_params.width),
        m_length(plane_params.length),
        m_number_columns(CalculateVerticiesCount(plane_params, false)),
        m_number_rows(CalculateVerticiesCount(plane_params, true)),
        m_hills_count(m_number_columns * m_number_rows),
        m_wstep(m_width / static_cast<float>(m_number_columns - 1)),
        m_lstep(m_length / static_cast<float>(m_number_rows - 1)),
        m_whalf(m_width * 0.5f),
        m_lhalf(m_length * 0.5f),
        m_poligons_count(m_base_mesh.indices.size() / 3),
        m_is_reflected(is_reflected),
        m_update_period(update_period_sec) {
    assert(m_base_mesh.indices.size() % 3 == 0);

    CalculateBaseHeights();

    m_calculated_mesh.vertices = m_base_mesh.vertices;
    m_calculated_mesh.indices  = m_base_mesh.indices;

    CalculateNewHillsHeights();
    UpdateMesh();
}

QCE::ErrorCode AnimateHills::Execute(QCE::CommandContext* context) {
    assert(context);
    assert(context->type == QCE::CommandContextType::E_CCT_DEFORMATED_MESH);
    assert(dynamic_cast<QCE::DeformatedMesh*>(context));
    auto ctx = static_cast<QCE::DeformatedMesh*>(context);

    if (ctx->update_mesh) {
        float elapsed_sec = 0.0f;

        if (m_timer.Check(m_update_period, elapsed_sec)) {
            CalculateNewHillsHeights();
            m_hills_hight = m_hills_hight_prev;
        }
        else {
            auto t = elapsed_sec / m_update_period;

            assert(t >= 0.0f && t <= 1.0f);

            for (size_t i = 0; i < m_hills_count; i++) {
                m_hills_hight[i] = m_hills_hight_prev[i] + (m_hills_hight_next[i] - m_hills_hight_prev[i]) * t;
            }
        }

        UpdateMesh();
    }

    ctx->deformation_result = &m_calculated_mesh;
    ctx->max_vertices_count = static_cast<uint32_t>(m_calculated_mesh.vertices.size());
    ctx->max_indeces_count  = static_cast<uint32_t>(m_calculated_mesh.indices.size());

    return QCE::ErrorCode::SUCCESS;
}

size_t AnimateHills::CalculateVerticiesCount(const QCE::PlaneParams& plane_params, bool rows) {
    if (plane_params.unit_squares) {
        if (rows)
            return static_cast<size_t>(plane_params.length) + 1;
        else
            return static_cast<size_t>(plane_params.width) + 1;
    }
    else
        return 2;
}

void AnimateHills::CalculateBaseHeights() {
    m_hills_base.reserve(m_hills_count);

    const auto start_x = m_is_reflected ?  m_whalf : -m_whalf;
    const auto step_x  = m_is_reflected ? -m_wstep :  m_wstep;
    auto is_finished_x = [&](float x) {
        if (m_is_reflected)
            return x < -m_whalf;
        else
            return x >  m_whalf;
    };

    int xi = 0;
    int zi = 0;

    for (float z =   m_lhalf;
               z >= -m_lhalf;
               z -=  m_lstep) {
        for (float x =  start_x;
                   !is_finished_x(x);
                   x += step_x) {
            auto y = std::sqrtf(x + m_whalf) * (std::sqrtf(0.05f * (z + m_lhalf)) + 0.5f);
            if (xi % 2 == 0 || zi % 2 == 0) {
                y *= 0.75f;
            }


            m_hills_base.push_back(y);

            xi++;
        }

        zi++;
    }

    assert(m_hills_base.size() == m_hills_count);

    m_hills_hight.resize(m_hills_count);
    m_hills_hight_next = m_hills_base;
}

void AnimateHills::CalculateNewHillsHeights() {
    m_hills_hight_prev.swap(m_hills_hight_next);

    assert(m_hills_base.size() == m_hills_count);
    m_hills_hight_next = m_hills_base;

    for (auto& height : m_hills_hight_next)
        height *= m_uniform_dist(m_prng);
}

void AnimateHills::UpdateMesh() {
    for (auto& vertex : m_calculated_mesh.vertices) {
        const auto x = vertex.position.x();
        const auto z = vertex.position.z();
        auto& y = vertex.position.y();

        // calculate index in hills cache
        const auto xi = static_cast<size_t>((m_whalf + x) / m_wstep);
        const auto zi = static_cast<size_t>((m_lhalf - z) / m_lstep);
        assert(xi < m_number_columns && zi < m_number_rows);

        const auto index = xi + zi * m_number_columns;
        assert(index < m_hills_hight.size());
        y = m_hills_hight[index];
    }

    assert(m_calculated_mesh.indices.size() % 3 == 0);
    for (size_t i = 0; i < m_calculated_mesh.indices.size(); i += 3) {
        auto i0 = m_calculated_mesh.indices[i];
        auto i1 = m_calculated_mesh.indices[i + 1];
        auto i2 = m_calculated_mesh.indices[i + 2];
        auto& v0 = m_calculated_mesh.vertices[i0];
        auto& v1 = m_calculated_mesh.vertices[i1];
        auto& v2 = m_calculated_mesh.vertices[i2];

        v0.normal = QCE::compute_normal(v0, v1, v2);
        v1.normal = v0.normal;
        v2.normal = v0.normal;
    }
}

