// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include <qce/objects/camera.hpp>

namespace QCE {
    Camera::Camera(
            const float3d& position,
            const float3d& target,
            const float3d& up,
            bool LH):
        m_position(position),
        m_target(target),
        m_up_direction(up),
        m_is_LH(LH) {
        UpdateViewMatrix();
    }

    void Camera::UpdateViewMatrix() const {
        assert(m_is_LH && "Only LH implemented");

        auto position = vector_init(m_position.arr);
        auto target   = vector_init(m_target.arr);
        auto up       = vector_init(m_up_direction.arr);

        auto view = camera_look_to_lh_matrix(position, target, up);
        matrix_copy(view, m_view_matrix.arr);
        m_need_recalc_view = false;
    }
}