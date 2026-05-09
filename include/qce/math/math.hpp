// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/vector.hpp>
#include <qce/math/matrix.hpp>
#include <qce/math/color.hpp>

#include <cmath>
#include <cstring>
#include <cassert>
#include <vector>

namespace QCE {
    static constexpr float PI     = 3.1415926535f;
    static constexpr float PI_DEG = 180.0f;

    struct float2d {
        float arr[2] = { 0.0f, 0.0f };

        float2d& operator=(const std::vector<float>& numbers) {
            assert(numbers.size() >= 2);
            std::memcpy(arr, numbers.data(), sizeof(float) * 2);
            return *this;
        }

        float& x() noexcept { return arr[0]; }
        float& y() noexcept { return arr[1]; }

        const float& x() const noexcept { return arr[0]; }
        const float& y() const noexcept { return arr[1]; }
    };

    struct float3d {
        float arr[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        float3d& operator=(const std::vector<float>& numbers) {
            assert(numbers.size() >= 3);
            std::memcpy(arr, numbers.data(), sizeof(float) * 3);
            return *this;
        }

        float& x() noexcept { return arr[0]; }
        float& y() noexcept { return arr[1]; }
        float& z() noexcept { return arr[2]; }

        const float& x() const noexcept { return arr[0]; }
        const float& y() const noexcept { return arr[1]; }
        const float& z() const noexcept { return arr[2]; }
    };

    struct float4d : public float3d {
        float4d& operator=(const std::vector<float>& numbers) {
            assert(numbers.size() >= 4);
            std::memcpy(arr, numbers.data(), sizeof(float) * 4);
            return *this;
        }

        float& w() noexcept { return arr[3]; }
        const float& w() const noexcept { return arr[3]; }
    };

    struct float4x4 {
        float arr[16] = {};

        float& operator()(ptrdiff_t row, ptrdiff_t col) noexcept {
            return arr[4 * row + col];
        }
        const float& operator()(ptrdiff_t row, ptrdiff_t col) const noexcept {
            return arr[4 * row + col];
        }
    };

    struct vertex {
        float3d position{};
        color_rgba color{};
        float2d texture_coordinates{};
    };

    static inline float constexpr deg_to_rad(float deg) {
        return (deg / PI_DEG) * PI;
    }
    static inline float constexpr rad_to_deg(float rad) {
        return (rad / PI) * PI_DEG;
    }
}
