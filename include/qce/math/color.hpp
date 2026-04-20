// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

namespace QCE {
    struct color_rgba
    {
        float arr[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        float& r() noexcept { return arr[0]; }
        float& g() noexcept { return arr[1]; }
        float& b() noexcept { return arr[2]; }
        float& a() noexcept { return arr[3]; }

        const float& r() const noexcept { return arr[0]; }
        const float& g() const noexcept { return arr[1]; }
        const float& b() const noexcept { return arr[2]; }
        const float& a() const noexcept { return arr[3]; }
    };

    static constexpr color_rgba WHITE =  { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr color_rgba BLACK =  { 0.0f, 0.0f, 0.0f, 1.0f };
    static constexpr color_rgba RED   =  { 1.0f, 0.0f, 0.0f, 1.0f };
    static constexpr color_rgba GREEN =  { 0.0f, 1.0f, 0.0f, 1.0f };
    static constexpr color_rgba BLUE  =  { 0.0f, 0.0f, 1.0f, 1.0f };
    static constexpr color_rgba YELLOW=  { 1.0f, 1.0f, 0.0f, 1.0f };
    static constexpr color_rgba CYAN  =  { 0.0f, 1.0f, 1.0f, 1.0f };
    static constexpr color_rgba MAGENTA= { 1.0f, 0.0f, 1.0f, 1.0f };
}
