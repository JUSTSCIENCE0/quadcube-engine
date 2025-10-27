// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include <qce/transform.hpp>

#include <cu/math-utils.hpp>

#include <cassert>

namespace QCE {
    Transform::Transform(const float4x4& start_matrix) :
        m_matrix(start_matrix) {
        UpdateSRT();
    }

    Transform::Transform(
            const quaternion& rotation,
            const float3d& position,
            const float3d& scale) :
        m_rotation(rotation),
        m_position(position),
        m_scale(scale) {
        NormalizeRotation();
        UpdateMatrix();
    }

    void Transform::NormalizeRotation() {
        auto rotation_quat = vector_init(m_rotation.arr);
        rotation_quat = vector_normalize(rotation_quat);
        vector_copy(rotation_quat, m_rotation.arr);
    }

    void Transform::UpdateMatrix() const {
        // scale
        auto S = matrix_init(
            m_scale.arr[0], 0.0f, 0.0f, 0.0f,
            0.0f, m_scale.arr[1], 0.0f, 0.0f,
            0.0f, 0.0f, m_scale.arr[2], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        // rotation
        auto rotation_quat = vector_init(m_rotation.arr);
        assert(CU::is_equal(1.0f, vector_length(rotation_quat)));
        auto R = quaternion_to_rotation_matrix(rotation_quat);

        // transition
        auto T = matrix_init(
            1.0f, 0.0f, 0.0f, m_position.arr[0],
            0.0f, 1.0f, 0.0f, m_position.arr[1],
            0.0f, 0.0f, 1.0f, m_position.arr[2],
            0.0f, 0.0f, 0.0f, 1.0f
        );

        // M = T * R * S
        auto M = matrix_mul(T, R);
        M = matrix_mul(M, S);

        m_need_recalc_matrix = false;
        matrix_copy(M, m_matrix.arr);
    }

    void Transform::UpdateSRT() {
        m_position.x() = m_matrix(0, 3);
        m_position.y() = m_matrix(1, 3);
        m_position.z() = m_matrix(2, 3);

        auto vector_x = vector_init(m_matrix(0, 0), m_matrix(1, 0), m_matrix(2, 0), 0.0f);
        auto vector_y = vector_init(m_matrix(0, 1), m_matrix(1, 1), m_matrix(2, 1), 0.0f);
        auto vector_z = vector_init(m_matrix(0, 2), m_matrix(1, 2), m_matrix(2, 2), 0.0f);

        m_scale.x() = vector_length(vector_x);
        m_scale.y() = vector_length(vector_y);
        m_scale.z() = vector_length(vector_z);

        vector_x /= m_scale.x();
        vector_y /= m_scale.y();
        vector_z /= m_scale.z();

        auto rotation_matrix = matrix_init(
            vector_x, vector_y, vector_z, vector_init(0.0f, 0.0f, 0.0f, 1.0f));
        auto quat = rotation_matrix_to_quaternion(rotation_matrix);
        vector_copy(quat, m_rotation.arr);
    }
}
