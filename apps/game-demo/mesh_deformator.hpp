// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/command.hpp>
#include <qce/objects/figures.hpp>

#include <qce/ancillary/timer.hpp>

#include <random>

using index_t = QCE::index_t;

struct AnimateHills : public QCE::BaseCommand {
    AnimateHills(
        const QCE::PlaneParams& plane_params,
        const QCE::Mesh& base_mesh,
        bool is_reflected = false);
    virtual ~AnimateHills() = default;

    QCE::ErrorCode Execute(QCE::CommandContext* context) override;

private:
    static size_t CalculateVerticiesCount(const QCE::PlaneParams& plane_params, bool row);
    void CalculateBaseHeights();
    void CalculateHillsHeights();
    void UpdateMesh();

    const QCE::Mesh& m_base_mesh;
    QCE::Mesh m_calculated_mesh{};
    std::vector<float> m_hills_base{};
    std::vector<float> m_hills_cache{};

    const float  m_width;
    const float  m_length;
    const size_t m_number_columns; // along width
    const size_t m_number_rows;    // along length
    const float  m_wstep;
    const float  m_lstep;
    const float  m_whalf;
    const float  m_lhalf;
    const size_t m_poligons_count;
    const bool   m_is_reflected;

    QCE::Timer m_timer{};

    std::random_device m_rd{};
    std::mt19937 m_prng{m_rd()};
    std::uniform_real_distribution<float> m_uniform_dist{0.25f, 1.25f};
};