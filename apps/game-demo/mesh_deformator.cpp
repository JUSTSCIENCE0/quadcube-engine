// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include "mesh_deformator.hpp"

#include <stdexcept>
#include <cmath>
#include <cstring>

AnimateHills::AnimateHills(
    const QCE::PlaneParams& plane_params,
    const QCE::Mesh& base_mesh,
    bool is_reflected) :
        QCE::BaseCommand("AnimateHills"),
        m_base_mesh(base_mesh),
        m_width(plane_params.width),
        m_length(plane_params.length),
        m_number_columns(CalculateVerticiesCount(plane_params, false)),
        m_number_rows(CalculateVerticiesCount(plane_params, true)),
        m_wstep(m_width / static_cast<float>(m_number_columns - 1)),
        m_lstep(m_length / static_cast<float>(m_number_rows - 1)),
        m_whalf(m_width * 0.5f),
        m_lhalf(m_length * 0.5f),
        m_poligons_count(m_base_mesh.indices.size() / 3),
        m_is_reflected(is_reflected) {
    assert(m_base_mesh.indices.size() % 3 == 0);

    CalculateBaseHeights();

    m_calculated_mesh.vertices = m_base_mesh.vertices;
    m_calculated_mesh.indices  = m_base_mesh.indices;
}

QCE::ErrorCode AnimateHills::Execute(QCE::CommandContext* context) {
    assert(context);
    assert(context->type == QCE::CommandContextType::E_CCT_DEFORMATED_MESH);
    assert(dynamic_cast<QCE::DeformatedMesh*>(context));

    CalculateHillsHeights();
    UpdateMesh();

    auto ctx = static_cast<QCE::DeformatedMesh*>(context);
    ctx->deformation_result = &m_calculated_mesh;

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
    const auto hills_count = m_number_columns * m_number_rows;
    m_hills_base.reserve(hills_count);

    for (float z =   m_lhalf;
               z >= -m_lhalf;
               z -=  m_lstep)
        for (float x = -m_whalf;
                   x <= m_whalf;
                   x += m_wstep) {
        auto y = std::sqrtf(x + m_whalf);
        m_hills_base.push_back(y);
    }

    assert(m_hills_base.size() == hills_count);
}

void AnimateHills::CalculateHillsHeights() {
    const auto hills_count = m_number_columns * m_number_rows;
    assert(m_hills_base.size() == hills_count);
    m_hills_cache.resize(hills_count);

    // TODO
    m_hills_cache = m_hills_base;
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
        assert(index < m_hills_cache.size());
        y = m_hills_cache[index];
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

