// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

// Currently, only single vector and matrix operations are implemented
// TODO: add batch math functions implementation
// to take full advantage of SIMD instructions

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
    static constexpr float _2PI   = 6.283185307f;
    static constexpr float SIN45  = 0.707106781f;

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

    using quaternion = float4d;

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
        float3d normal{};
        float2d texture_coordinates{};
    };

    static inline float3d compute_normal(const vertex& v0, const vertex& v1, const vertex& v2) noexcept {
        auto p0 = vector_init(v0.position.arr);
        auto p1 = vector_init(v1.position.arr);
        auto p2 = vector_init(v2.position.arr);
        auto edge1 = p1 - p0;
        auto edge2 = p2 - p0;
        auto normal = vector_cross_product(edge1, edge2);
        normal = vector_normalize(normal);

        float3d result;
        vector_copy(normal, result.arr);
        return result;
    }

    static inline float constexpr deg_to_rad(float deg) noexcept {
        return (deg / PI_DEG) * PI;
    }
    static inline float constexpr rad_to_deg(float rad) noexcept {
        return (rad / PI) * PI_DEG;
    }

    static inline uint32_t constexpr quantize(float value, float min, float max, uint32_t bits) noexcept {
        const auto range = max - min;
        const auto scale = (1 << bits) - 1;
        return static_cast<uint32_t>((value - min) / range * scale);
    }
    static inline float constexpr dequantize(uint32_t value, float min, float max, uint32_t bits) noexcept {
        const auto range = max - min;
        const auto scale = (1 << bits) - 1;
        return static_cast<float>(value) / scale * range + min;
    }

    static inline quaternion euler_rad_to_quaternion(float roll, float pitch, float yaw) noexcept {
        auto angles = vector_init(roll, pitch, yaw, 0.0f);
        auto quat = quaternion_from_euler_rad(angles);
        quaternion result{};
        vector_copy(quat, result.arr);
        return result;
    }
    static inline quaternion euler_deg_to_quaternion(float roll, float pitch, float yaw) noexcept {
        roll  = deg_to_rad(roll);
        pitch = deg_to_rad(pitch);
        yaw   = deg_to_rad(yaw);
        return euler_rad_to_quaternion(roll, pitch, yaw);
    }

    static inline void normalize_quaternion(quaternion& quat) noexcept {
        auto rotation_quat = vector_init(quat.arr);
        rotation_quat = vector_normalize(rotation_quat);
        vector_copy(rotation_quat, quat.arr);
    }

    static inline uint32_t compress_quaternion(const quaternion& q) noexcept {
        float tmp[4] = {
            std::fabs(q.x()), std::fabs(q.y()),
            std::fabs(q.z()), std::fabs(q.w())
        };
        const auto max_abs   = std::max_element(tmp, tmp + 4);
        const auto max_index = static_cast<uint32_t>(std::distance(tmp, max_abs));

        const auto max_it = &q.arr[max_index];
        const auto k = (*max_it < 0.0f) ? -1.0f : 1.0f;

        int i = 0;
        for (auto it = q.arr; it != q.arr + 4; ++it) {
            if (it == max_it)
                continue;
            tmp[i++] = (*it) * k;
        }

        constexpr uint32_t BITS_PER_COMPONENT = 10;
        constexpr uint32_t MAX_INDEX_SHIFT = 30;

        uint32_t result = max_index << MAX_INDEX_SHIFT;
        result |= (quantize(tmp[0], -SIN45, SIN45, BITS_PER_COMPONENT) << 20);
        result |= (quantize(tmp[1], -SIN45, SIN45, BITS_PER_COMPONENT) << 10);
        result |= (quantize(tmp[2], -SIN45, SIN45, BITS_PER_COMPONENT));
        return result;
    }

    static inline quaternion decompress_quaternion(uint32_t compressed) noexcept {
        constexpr uint32_t BITS_PER_COMPONENT = 10;
        constexpr uint32_t MAX_INDEX_SHIFT = 30;
        constexpr uint32_t COMPONENT_MASK = (1 << BITS_PER_COMPONENT) - 1;

        const auto max_index = compressed >> MAX_INDEX_SHIFT;
        const float tmp[3] = {
            dequantize((compressed >> 20) & COMPONENT_MASK, -SIN45, SIN45, BITS_PER_COMPONENT),
            dequantize((compressed >> 10) & COMPONENT_MASK, -SIN45, SIN45, BITS_PER_COMPONENT),
            dequantize(compressed         & COMPONENT_MASK, -SIN45, SIN45, BITS_PER_COMPONENT)
        };
        const float max_value = std::sqrtf(1.0f - (tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]));

        quaternion result{};
        for (uint32_t i = 0, j = 0; i < 4; ++i) {
            if (i == max_index) {
                result.arr[i] = max_value;
            }
            else {
                result.arr[i] = tmp[j++];
            }
        }
        return result;
    }

    static inline float constexpr lerp(float a, float b, float t) noexcept {
        return a + (b - a) * t;
    }
    static inline void lerp(const float* a, const float* b, float t, float* result) noexcept {
        assert(a && b && result);

        auto av = vector_init(a);
        auto bv = vector_init(b);
        auto rv = av + ((bv - av) * t);
        vector_copy(rv, result);
    }
    static inline void lerp16(const float* a, const float* b, float t, float* result) noexcept {
        assert(a && b && result);

        auto am = matrix_init(a);
        auto bm = matrix_init(b);
        auto tm = matrix_init(
            t, t, t, t,
            t, t, t, t,
            t, t, t, t,
            t, t, t, t);
        auto rm = am + matrix_hadamard_mul(bm - am, tm);
        matrix_copy(rm, result);
    }
    static inline void nlerp(const float* a, const float* b, float t, float* result) noexcept {
        assert(a && b && result);

        auto av = vector_init(a);
        auto bv = vector_init(b);

        if (vector_dot_product(av, bv) < 0.0f)
            av = -av;

        auto rv = av + ((bv - av) * t);
        rv = vector_normalize(rv);
        vector_copy(rv, result);
    }
    static inline void slerp(const float* a, const float* b, float t, float* result) noexcept {
        assert(a && b && result);

        auto av = vector_init(a);
        auto bv = vector_init(b);

        auto dot = vector_dot_product(av, bv);
        if (dot < 0.0f) {
            av = -av;
            dot = -dot;
        }
        if (dot > 0.9995f) { // Use linear interpolation for close vectors
            nlerp(a, b, t, result);
            return;
        }

        const auto theta = std::acosf(dot);
        const auto sin_theta = std::sinf(theta);
        const auto ak = std::sinf((1.0f - t) * theta) / sin_theta;
        const auto bk = std::sinf(t * theta) / sin_theta;

        auto rv = av * ak + bv * bk;
        vector_copy(rv, result);
    }
}
