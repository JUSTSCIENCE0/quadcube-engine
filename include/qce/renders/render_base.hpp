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
    CU_END_ENUM(WindowMode)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct WindowConfig {
        WindowMode mode = WindowMode::E_REGULAR_WINDOW;

        int width = 1280;
        int height = 720;
    };

    class BaseWindow {
    public:
        explicit BaseWindow(WindowConfig initial_config) :
            m_config(std::move(initial_config)) {}

        BaseWindow(const BaseWindow&) = delete;
        BaseWindow(BaseWindow&&) = delete;
        BaseWindow& operator=(const BaseWindow&) = delete;
        BaseWindow& operator=(BaseWindow&&) = delete;
        virtual ~BaseWindow() = default;

        virtual ErrorCode UpdateConfig(WindowConfig config) {
            m_config = std::move(config);
            return ErrorCode::SUCCESS;
        }

        virtual ErrorCode MainLoop() = 0;

    protected:
        WindowConfig m_config{};
    };
}
