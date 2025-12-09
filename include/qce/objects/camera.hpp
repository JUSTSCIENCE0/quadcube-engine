// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/components/transform.hpp>
#include <qce/components/movement.hpp>

namespace QCE {
    class Camera {
    public:
        explicit Camera(bool LH = true);
        //Camera(const Transform& transform, bool LH = true);

        void SetFoV(float fov_rad) {
            assert(fov_rad > deg_to_rad(1));
            m_fov_rad = fov_rad;
            m_need_recalc_proj = true;
        }
        void SetAspect(float aspect) {
            assert(aspect > 0.0f);
            m_aspect = aspect;
            m_need_recalc_proj = true;
        }
        void SetZLimits(float znear, float zfar) {
            assert(zfar > znear);
            m_znear = znear;
            m_zfar = zfar;
            m_need_recalc_proj = true;
        }

        void Move(Velocity velocity, double delta_time);
        void RotateView(float d_yaw, float d_pitch);

        /// Add transform to current
        //void MovePosition(const float3d& translation);
        //void MoveTarget(const float3d& translation);
        //void RotateView(const float3d& angels); // move target
        //void RotateAround(const float3d& angels); // move position

        /// Overwrite transform
        void SetPosition(const float3d& position) {
            m_position = position;
            m_need_recalc_fru = true;
        }
        void SetTarget(const float3d& target) {
            m_target = target;
            m_need_recalc_fru = true;
        }
        void SetUp(const float3d& up) {
            m_up_direction = up;
            m_need_recalc_fru = true;
        }

        const float4x4& GetView() const;
        const float4x4& GetProj() const;

    private:
        void UpdateViewMatrix() const;
        void UpdateProjMatrix() const;
        void LoadParams(
            vector& position, vector& target, vector& forward, vector& right, vector& up_real) const;

        float3d m_position     = { 2.0f, 2.0f, -2.0f };
        float3d m_target       = { 0.0f, 0.0f, 0.0f };
        float3d m_up_direction = { 0.0f, 1.0f, 0.0f };

        float m_fov_rad = deg_to_rad(60);
        float m_aspect  = 16.0f / 9.0f;
        float m_znear   = 1.0f;
        float m_zfar    = 1000.0f;

        const bool m_is_LH = true;

        // cache
        mutable float4x4 m_view_matrix{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            }
        };
        mutable bool m_need_recalc_view = true;

        mutable float4x4 m_proj_matrix{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 0.0f
            }
        };
        mutable bool m_need_recalc_proj = true;

        mutable float3d m_forward = {};
        mutable float3d m_right = {};
        mutable float3d m_actual_up = {};
        mutable bool m_need_recalc_fru = true;

        mutable Transform m_transform{};
        mutable bool m_need_update_transform = true;

        float m_yaw = 0.0f;
        float m_pitch = 0.0f;
        mutable bool m_need_update_yaw_pitch = true;
        static constexpr float MAX_PITCH_ABS = deg_to_rad(89);
    };
}

