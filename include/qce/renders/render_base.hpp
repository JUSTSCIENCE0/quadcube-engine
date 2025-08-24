// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/error_codes.hpp>

#include <cstring>
#include <stdint.h>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(WindowMode) \
        CU_ENUM_UNIT(E_NORMAL_WINDOW) \
        CU_ENUM_UNIT(E_FRAMELESS_WINDOW) \
        CU_ENUM_UNIT(E_FULLSCREEN_WINDOW) \
    CU_END_ENUM(WindowMode)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct WindowConfig {
        WindowMode mode = WindowMode::E_NORMAL_WINDOW;

        int width = 1280;
        int height = 720;
    };

    class IRenderWindow {
        virtual ErrorCode Init() = 0;
    };
}
