// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/error_codes.hpp>

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
    CU_END_ENUM(WindowMode) \
    CU_BEGIN_ENUM(RenderType) \
        CU_ENUM_UNIT(E_RENDER_DIRECTX12) \
        CU_ENUM_UNIT(E_RENDER_VULKAN) \
    CU_END_ENUM(RenderType)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

#ifdef WIN32
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_DIRECTX12;
#else
    static constexpr auto DEFAULT_RENDER_TYPE = RenderType::E_RENDER_VULKAN;
#endif

    struct WindowConfig {
        WindowMode mode = WindowMode::E_REGULAR_WINDOW;

        int width = 1280;
        int height = 720;
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

    protected:
        explicit RenderBase(RenderConfig initial_config) :
            m_config(std::move(initial_config)) {
        }

        RenderConfig m_config{};
    };

    std::shared_ptr<RenderBase> GetRender(RenderConfig config, void* window, void* app);
}
