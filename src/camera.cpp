// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include <qce/objects/camera.hpp>

#include <iostream>

namespace QCE {
    Camera::Camera(bool LH):
        m_is_LH(LH) {}

    void Camera::UpdateFRU(
            vector& position, vector& forward, vector& right, vector& up_real) const {
        position = vector_init(m_position.arr);
        auto target = vector_init(m_target.arr);
        auto up = vector_init(m_up_direction.arr);

        forward = vector_normalize(target - position);
        right = vector_normalize(vector_cross_product(up, forward));
        up_real = vector_cross_product(forward, right);

        vector_copy(forward, m_forward.arr);
        vector_copy(right, m_right.arr);
        vector_copy(up_real, m_actual_up.arr);

        m_need_recalc_fru = false;
    }

    void Camera::UpdateViewMatrix() const {
        assert(m_is_LH && "Only LH implemented");

        vector position, forward, right, up_real;

        if (m_need_recalc_fru) {
            UpdateFRU(position, forward, right, up_real);
        }
        else {
            position = vector_init(m_position.arr);
            forward = vector_init(m_forward.arr);
            right = vector_init(m_right.arr);
            up_real = vector_init(m_actual_up.arr);
        }

        auto view = camera_look_to_lh_matrix(position, forward, right, up_real);
        matrix_copy(view, m_view_matrix.arr);
        m_need_recalc_view = false;
    }

    void Camera::UpdateProjMatrix() const {
        assert(m_is_LH && "Only LH implemented");

        float height = 1.0f / (std::tanf(m_fov_rad * 0.5f));
        float width = height / m_aspect;
        float range_z = m_zfar / (m_zfar - m_znear);

        m_proj_matrix(0, 0) = width;
        m_proj_matrix(1, 1) = height;
        m_proj_matrix(2, 2) = range_z;
        m_proj_matrix(3, 2) = -1.0f * m_znear * range_z;
        m_need_recalc_proj = false;
    }

    void Camera::Move(Velocity velocity, double delta_time) {
        auto move = velocity.GetMovement(delta_time);

        if (!move.x() && !move.z())
            return;

        vector position, forward, right, up_real;

        if (m_need_recalc_fru) {
            UpdateFRU(position, forward, right, up_real);
        }
        else {
            position = vector_init(m_position.arr);
            forward = vector_init(m_forward.arr);
            right = vector_init(m_right.arr);
            up_real = vector_init(m_actual_up.arr);
        }

        auto delta = (forward * move.z()) + (right * move.x());
        position += delta;
        vector_copy(position, m_position.arr);
        m_need_recalc_view = true;

        //std::cout << "Camera move: " <<
        //    move.x() << " " <<
        //    move.z() << std::endl;
    }

    const float4x4& Camera::GetView() const {
        if (m_need_recalc_view)
            UpdateViewMatrix();
        return m_view_matrix;
    }

    const float4x4& Camera::GetProj() const {
        if (m_need_recalc_proj)
            UpdateProjMatrix();
        return m_proj_matrix;
    }
}