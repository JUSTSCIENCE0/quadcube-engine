// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>
#include <qce/objects/scene.hpp>
#include <qce/renders/render_type.hpp>

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

    struct RenderConfig {
        RenderType render_type = DEFAULT_RENDER_TYPE;

        int width = 1280;
        int height = 720;
    };

    class RenderBase {
    public:
        RenderBase(const RenderBase&) = delete;
        RenderBase(RenderBase&&) = delete;
        RenderBase& operator=(const RenderBase&) = delete;
        RenderBase& operator=(RenderBase&&) = delete;
        virtual ~RenderBase() = default;

        virtual ErrorCode UpdateConfig(RenderConfig config) {
            m_config = std::move(config);
            return ErrorCode::SUCCESS;
        }

        ErrorCode Draw() {
            assert(DerivedDraw);
            return DerivedDraw(this);
        }

        ErrorCode SetCurrentScene(Scene* scene) {
            m_current_scene = scene;

            QCE_CRITICAL(UpdateCpuScene());

            assert(UpdateGpuScene);
            return UpdateGpuScene(this);
        }

    protected:
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

        template <typename /*TODO: concept*/ DerivedRender>
        explicit RenderBase(std::in_place_type_t<DerivedRender>, RenderConfig initial_config) :
            m_config(std::move(initial_config)),
            DerivedDraw([](void* ptr) { return static_cast<DerivedRender*>(ptr)->Draw(); }),
            UpdateGpuScene([](void* ptr) { return static_cast<DerivedRender*>(ptr)->UpdateGpuScene(); }) {
        }

        RenderConfig m_config{};

        Scene* m_current_scene = nullptr;
        RenderSceneCPU m_scene_cpu{};

    private:
        ErrorCode UpdateCpuScene();

        ErrorCode(*DerivedDraw)(void*) = nullptr;
        ErrorCode(*UpdateGpuScene)(void*) = nullptr;
    };

    std::shared_ptr<RenderBase> GetRender(RenderConfig config, void* window, void* app);
}
