// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <cmath>

#include <gtest/gtest.h>

#include <qce/loaders/binary/compression.hpp>

namespace QCE::test {
    inline void check_color_rgba(const color_rgba& lhs, const color_rgba& rhs) {
        EXPECT_NEAR(lhs.r(), rhs.r(), 1.0f / 1023.0f);
        EXPECT_NEAR(lhs.g(), rhs.g(), 1.0f / 1023.0f);
        EXPECT_NEAR(lhs.b(), rhs.b(), 1.0f / 1023.0f);
        EXPECT_NEAR(lhs.a(), rhs.a(), 1.0f / 1023.0f);
    }

    inline float get_quaternion_dot_error(QuaternionQuantization quantization) {
        switch (quantization) {
        case E_32_BIT_QQ: {
            // empirically found values
            // TODO: find values from verified sources
            constexpr float AngularError = deg_to_rad(0.25f);
            return std::cos(AngularError / 2);
        }
        case E_64_BIT_QQ: {
            constexpr float AngularError = deg_to_rad(0.0005f);
            return std::cos(AngularError / 2);
        }
        default:
            return 0.0f;
        }
    }

    inline void check_quaternions(
        const quaternion& lhs,
        const quaternion& rhs,
        QuaternionQuantization quantization) {
        if (quantization == E_NO_QQ) {
            EXPECT_FLOAT_EQ(lhs.x(), rhs.x()) << "Quaternion error";
            EXPECT_FLOAT_EQ(lhs.y(), rhs.y()) << "Quaternion error";
            EXPECT_FLOAT_EQ(lhs.z(), rhs.z()) << "Quaternion error";
            EXPECT_FLOAT_EQ(lhs.w(), rhs.w()) << "Quaternion error";
        }
        else {
            const float dot = std::abs(
                lhs.x() * rhs.x() +
                lhs.y() * rhs.y() +
                lhs.z() * rhs.z() +
                lhs.w() * rhs.w());

            EXPECT_NEAR(dot, 1.0f, get_quaternion_dot_error(quantization)) << "Quaternion error";
        }
    }

    inline void check_float3d(const float3d& lhs, const float3d& rhs, float eps) {
        EXPECT_NEAR(lhs.x(), rhs.x(), eps) << "Float3D error";
        EXPECT_NEAR(lhs.y(), rhs.y(), eps) << "Float3D error";
        EXPECT_NEAR(lhs.z(), rhs.z(), eps) << "Float3D error";
    }
}
