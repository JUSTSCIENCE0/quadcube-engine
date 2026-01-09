// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>
#include <qce/objects/resource_manager.hpp>
#include <qce/configs/render_config.hpp>
#include <qce/ecs/ecs.hpp>

#include <array>
#include <utility>
#include <cstring>
#include <string>
#include <stdint.h>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(WindowMode) \
        CU_ENUM_UNIT(E_REGULAR_WINDOW) \
        CU_ENUM_UNIT(E_FRAMELESS_WINDOW) \
        CU_ENUM_UNIT(E_FULLSCREEN_WINDOW) \
    CU_END_ENUM(WindowMode)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct GraphicsOutputConfig {
        WindowMode mode = WindowMode::E_REGULAR_WINDOW;

        int width = 1280;
        int height = 720;

        std::wstring caption = L"QCE Application";

        bool is_first_person = false;
    };

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

        virtual ErrorCode UpdateGpuScene() = 0;

        Entities& m_entities;
        RenderConfig m_config{};

        RenderSceneCPU m_scene_cpu{};

        ShaderIndeces m_shader_indeces{};

    private:
        ErrorCode UpdateCpuScene();
    };

    std::shared_ptr<RenderBase> GetRender(
        Entities& entities, RenderConfig config, void* window, void* app);
}
