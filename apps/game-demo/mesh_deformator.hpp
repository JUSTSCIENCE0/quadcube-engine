// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>
#include <qce/configs/configs_traits.hpp>
#include <qce/objects/command.hpp>
#include <qce/objects/figures.hpp>

#include <qce/ancillary/timer.hpp>

#include <random>

using index_t = QCE::index_t;

class HillsAnimationSystem;
struct HillsAnimationConfig {};
namespace QCE {
    template<>
    struct config_corresponding_system<HillsAnimationConfig> {
        using System = HillsAnimationSystem;
    };
}

class HillsAnimationSystem {
public:
    using Config = HillsAnimationConfig;

    explicit HillsAnimationSystem(QCE::Entities& entities) :
        m_entities(entities) {}

    QCE::ErrorCode Setup() {
        return QCE::ErrorCode::SUCCESS;
    }

    QCE::ErrorCode Update();

private:
    QCE::Entities& m_entities;
};

struct AnimateHills : public QCE::BaseCommand {
    AnimateHills(
        float update_period_sec,
        const QCE::PlaneParams& plane_params,
        const QCE::Mesh& base_mesh,
        bool is_reflected = false);
    virtual ~AnimateHills() = default;

    QCE::ErrorCode Execute(QCE::CommandContext* context) override;

private:
    static size_t CalculateVerticiesCount(const QCE::PlaneParams& plane_params, bool row);
    void CalculateBaseHeights();
    void CalculateNewHillsHeights();
    void UpdateMesh();

    const QCE::Mesh& m_base_mesh;
    QCE::Mesh m_calculated_mesh{};
    std::vector<float> m_hills_base{};
    std::vector<float> m_hills_hight{};
    std::vector<float> m_hills_hight_prev{};
    std::vector<float> m_hills_hight_next{};

    const float  m_width;
    const float  m_length;
    const size_t m_number_columns; // along width
    const size_t m_number_rows;    // along length
    const size_t m_hills_count;
    const float  m_wstep;
    const float  m_lstep;
    const float  m_whalf;
    const float  m_lhalf;
    const size_t m_poligons_count;
    const bool   m_is_reflected;

    const float  m_update_period; // seconds
    QCE::Timer m_timer{};

    std::random_device m_rd{};
    std::mt19937 m_prng{m_rd()};
    std::uniform_real_distribution<float> m_uniform_dist{0.25f, 1.25f};
};