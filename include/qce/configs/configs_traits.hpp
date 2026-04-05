// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

namespace QCE {
    template<typename T>
    struct config_corresponding_system {
        static_assert(false, "System for this config not found or not implemented");
    };

    template<typename T>
    using config_corresponding_system_t = config_corresponding_system<T>::System;

    struct CameraConfig;
    class CameraSystem;
    template<>
    struct config_corresponding_system<CameraConfig> {
        using System = CameraSystem;
    };

    struct HidConfig;
    class HidSystem;
    template<>
    struct config_corresponding_system<HidConfig> {
        using System = HidSystem;
    };

    struct MovementConfig;
    class MovementSystem;
    template<>
    struct config_corresponding_system<MovementConfig> {
        using System = MovementSystem;
    };

    struct RenderConfig;
    class RenderSystem;
    template<>
    struct config_corresponding_system<RenderConfig> {
        using System = RenderSystem;
    };
}
