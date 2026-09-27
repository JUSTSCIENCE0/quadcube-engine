// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <cstring>

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

    static constexpr color_rgba WHITE      = {  1.0f,  1.0f,  1.0f, 1.0f };
    static constexpr color_rgba BLACK      = {  0.0f,  0.0f,  0.0f, 1.0f };
    static constexpr color_rgba RED        = {  1.0f,  0.0f,  0.0f, 1.0f };
    static constexpr color_rgba GREEN      = {  0.0f,  1.0f,  0.0f, 1.0f };
    static constexpr color_rgba BLUE       = {  0.0f,  0.0f,  1.0f, 1.0f };
    static constexpr color_rgba YELLOW     = {  1.0f,  1.0f,  0.0f, 1.0f };
    static constexpr color_rgba CYAN       = {  0.0f,  1.0f,  1.0f, 1.0f };
    static constexpr color_rgba MAGENTA    = {  1.0f,  0.0f,  1.0f, 1.0f };
    static constexpr color_rgba GRAY       = {  0.5f,  0.5f,  0.5f, 1.0f };
    static constexpr color_rgba LIGHT_GRAY = { 0.75f, 0.75f, 0.75f, 1.0f };

    static inline bool color_by_name(const char* color_name, QCE::color_rgba& val) {
        assert(color_name);

#define MJSON_COLOR_READER(name) \
        if (!std::strcmp( #name , color_name)) { \
            val = QCE::name; \
            return true; \
        }
        MJSON_COLOR_READER(WHITE);
        MJSON_COLOR_READER(BLACK);
        MJSON_COLOR_READER(RED);
        MJSON_COLOR_READER(GREEN);
        MJSON_COLOR_READER(BLUE);
        MJSON_COLOR_READER(YELLOW);
        MJSON_COLOR_READER(CYAN);
        MJSON_COLOR_READER(MAGENTA);
        MJSON_COLOR_READER(GRAY);
        MJSON_COLOR_READER(LIGHT_GRAY);
#undef MJSON_COLOR_READER
        return false;
    }

    static inline bool color_by_hex(const char* color_hex, QCE::color_rgba& val) {
        if (std::strlen(color_hex) != 9 || color_hex[0] != '#')
            return false;

        auto hex = [](char c) {
            if (c >= '0' && c <= '9')
                return c - '0';
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
            if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
            return -1;
        };
        auto parse_byte = [&hex](const char* data, size_t pos, float& value) {
            const int hi = hex(data[pos]);
            const int lo = hex(data[pos + 1]);

            if (hi < 0 || lo < 0)
                return false;

            value = static_cast<float>((hi << 4) | lo) / 255.0f;
            return true;
        };

        return parse_byte(color_hex, 1, val.r()) &&
               parse_byte(color_hex, 3, val.g()) &&
               parse_byte(color_hex, 5, val.b()) &&
               parse_byte(color_hex, 7, val.a());;
    }
}
