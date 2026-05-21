// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include "configs_implementation.hpp"

#include <qce/ancillary/error_codes.hpp>
#include <qce/objects/resource_manager.hpp>
#include <qce/ecs/ecs.hpp>

#include <array>
#include <utility>
#include <cstring>
#include <string>
#include <stdint.h>

namespace QCE {
    class RenderBase {
    public:
        RenderBase(Entities& entities, RenderConfig initial_config) :
            m_entities(entities),
            m_config(std::move(initial_config)) {
            m_shader_indeces.fill(ResourceManager::INVALID_RESOURCE_INDEX);
        }

        RenderBase(const RenderBase&) = delete;
        RenderBase(RenderBase&&) = delete;
        RenderBase& operator=(const RenderBase&) = delete;
        RenderBase& operator=(RenderBase&&) = delete;
        virtual ~RenderBase() = default;

        virtual ErrorCode UpdateConfig(RenderConfig config) {
            m_config = std::move(config);
            return ErrorCode::SUCCESS;
        }

        virtual ErrorCode Draw() = 0;

        ErrorCode UpdateScene() {
            QCE_CRITICAL(UpdateCpuScene());

            return UpdateGpuScene();
        }

        ErrorCode UseShader(const std::string& name, ShaderType type);

    protected:
        using ShaderIndeces = std::array<
            size_t /* index*/,
            ShaderType::E_SHADERS_TYPE_COUNT>;
        struct RenderSceneCPU {
            struct Unit {
                uint32_t indeces_count = 0;
                uint32_t index_offset = 0;
                uint32_t vertex_offset = 0;
            };

            std::vector<vertex>  vertex_buffer;
            std::vector<index_t> index_buffer;

            uint32_t vertex_buffer_size = 0;
            uint32_t index_buffer_size = 0;
            constexpr static uint32_t VERTEX_STRIDE = sizeof(vertex);

            std::vector<Unit> units;
        };

        struct UnitConstants {
            float world_matrix[16];
        };

        struct PassConstants
        {
            float view_matrix[16] = {};
            float view_matrix_inv[16] = {};
            float proj_matrix[16] = {};
            float proj_matrix_inv[16] = {};
            float view_proj_matrix[16] = {};
            float view_proj_matrix_inv[16] = {};
            float eye_position[3] = { 0.0f, 0.0f, 0.0f };
            float render_target_size[2] = { 0.0f, 0.0f };
            float render_target_size_inv[2] = { 0.0f, 0.0f };
            float near_z = 0.0f;
            float far_z = 0.0f;
            float delta_time = 0.0f;
        };

        static constexpr int FRAME_RESOURCE_COUNT = 3;

        virtual ErrorCode UpdateGpuScene() = 0;

        Entities& m_entities;
        RenderConfig m_config{};

        RenderSceneCPU m_scene_cpu{};

        ShaderIndeces m_shader_indeces{};

    private:
        ErrorCode UpdateCpuScene();
    };
}
