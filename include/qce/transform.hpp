// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math.hpp>

namespace QCE {
    class Transform final {
    public:
        using quaternion = float4d;

        Transform() = default;
        Transform(const float4x4& start_matrix);
        Transform(
            const quaternion& quaternion,
            const float3d& position,
            const float3d& scale);
        // Transform(
        //    const float3d& angles,
        //    const float3d& position,
        //    const float3d& scale) { /*TODO*/ }

        /// Add transform to current
        // void Move(const float3d& translation) { /*TODO*/ }
        // void Rotate(const float3d& angles) { /*TODO*/ }
        // void Scale(const float3d& scale) { /*TODO*/ }

        /// Overwrite transform
        // void SetMatrix() { /*TODO*/ }
        // void MoveTo(const float3d& position) { /*TODO*/ }
        // void SetRotation(const float3d& angles) { /*TODO*/ }
        // void SetRotation(const quaternion& rotation) { /*TODO*/ }
        // void Rescale(const float3d& scale) { /*TODO*/ }

        /// Getters
        const float4x4& Matrix() const { return m_matrix; }
        // float3d Rotation() const;
        // const quaternion& RotationQuaternion() const;
        // const float3d& Position() const;
        // const float3d& Scale() const;

    private:
        void NormalizeRotation();
        void UpdateMatrix() const;
        void UpdateSRT();

        // cache
        mutable float4x4 m_matrix{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            }
        };
        mutable bool m_need_recalc_matrix = false;

        quaternion m_rotation{};
        float3d    m_position{};
        float3d    m_scale = { 1.0f, 1.0f, 1.0f };
    };
}
