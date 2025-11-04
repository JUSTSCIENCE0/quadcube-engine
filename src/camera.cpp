// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include <qce/objects/camera.hpp>

namespace QCE {
    Camera::Camera(bool LH):
        m_is_LH(LH) {}

    void Camera::UpdateViewMatrix() const {
        assert(m_is_LH && "Only LH implemented");

        auto position = vector_init(m_position.arr);
        auto target   = vector_init(m_target.arr);
        auto up       = vector_init(m_up_direction.arr);

        auto view = camera_look_to_lh_matrix(position, target, up);
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