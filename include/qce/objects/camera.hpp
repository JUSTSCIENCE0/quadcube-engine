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

        const float4x4& GetView() const;
        const float4x4& GetProj() const;

    private:
        void UpdateViewMatrix() const;
        void UpdateProjMatrix() const;

        float3d m_position{};
        float3d m_target{};
        float3d m_up_direction{ 0.0f, 1.0f, 0.0f };

        float m_fov_rad = deg_to_rad(60);
        float m_aspect = 16.0f / 9.0f;
        float m_znear = 1.0f;
        float m_zfar = 1000.0f;

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

        mutable float4x4 m_proj_matrix{
            .arr = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 0.0f
            }
        };
        mutable bool m_need_recalc_proj = false;
    };
}

