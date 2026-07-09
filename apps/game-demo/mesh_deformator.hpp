// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>
#include <qce/configs/configs_traits.hpp>
#include <qce/ancillary/structs.hpp>

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

    QCE::ErrorCode UpdateScene();

    QCE::ErrorCode Update();

private:
    struct AnimationCache {
        std::vector<float> hills_base{};
        std::vector<float> hills_hight{};
        std::vector<float> hills_hight_prev{};
        std::vector<float> hills_hight_next{};

        const float  width;
        const float  length;
        const size_t number_columns; // along width
        const size_t number_rows;    // along length
        const size_t hills_count;
        const float  wstep;
        const float  lstep;
        const float  whalf;
        const float  lhalf;
        const size_t poligons_count;
        const bool   is_reflected;

        QCE::Timer m_timer{};
    };
    std::vector<AnimationCache> m_animation_cache{};
    QCE::BufferMap m_animation_cache_map{};

    struct RandomGenerator {
        RandomGenerator() = default;
        RandomGenerator(const RandomGenerator&):
            m_rd(),
            m_prng(m_rd()),
            m_uniform_dist(0.25f, 1.25f) {}

        float get() {
            return m_uniform_dist(m_prng);
        }

    private:
        std::random_device m_rd{};
        std::mt19937 m_prng{ m_rd() };
        std::uniform_real_distribution<float> m_uniform_dist{ 0.25f, 1.25f };
    } m_rnd;

    QCE::Entities& m_entities;

    static size_t CalculateVerticiesCount(const QCE::PlaneParams& plane_params, bool row);
    void CalculateBaseHeights(AnimationCache& out);
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