// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/transform.hpp>

namespace QCE {
    class Camera {
    public:
        Camera(const float3d& position,
               const float3d& target,
               const float3d& up,
               bool LH = true);
        //Camera(const Transform& transform, bool LH = true);

        /// Add transform to current
        //void Move(const float3d& translation); // both target & position
        //void MovePosition(const float3d& translation);
        //void MoveTarget(const float3d& translation);
        //void RotateView(const float3d& angels); // move target
        //void RotateAround(const float3d& angels); // move position

        /// Overwrite transform
        //void SetPosition(const float3d& position);
        //void SetTarget(const float3d& target);
        //void SetUp(const float3d& up);

    private:
        void UpdateViewMatrix() const;

        float3d m_position{};
        float3d m_target{};
        float3d m_up_direction{ 0.0f, 1.0f, 0.0f };

        bool m_is_LH = true;

        // cache
        mutable float4x4 m_view_matrix{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            }
        };
        mutable bool m_need_recalc_view = false;
        mutable Transform m_transform{};
        mutable bool m_need_update_transform = false;
    };
}

