// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include <qce/components/transform.hpp>

#include <cu/math-utils.hpp>

#include <cassert>

namespace QCE {
    void normalize_quaternion(quaternion& quat) {
        auto rotation_quat = vector_init(quat.arr);
        rotation_quat = vector_normalize(rotation_quat);
        vector_copy(rotation_quat, quat.arr);
    }

    void calculate_transform_matrix(
            const TransformComponents& components, TransformMatrix& out) {
        // scale
        auto scale = components.scale.arr;
        auto S = matrix_init(
            scale[0], 0.0f, 0.0f, 0.0f,
            0.0f, scale[1], 0.0f, 0.0f,
            0.0f, 0.0f, scale[2], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        // rotation
        auto rotation_quat = vector_init(components.rotation.arr);
        assert(CU::is_equal(1.0f, vector_length(rotation_quat)));
        auto R = quaternion_to_rotation_matrix(rotation_quat);

        // transition
        auto position = components.position.arr;
        auto T = matrix_init(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            position[0],
            position[1],
            position[2], 1.0f
        );

        // M = S * R * T
        auto M = matrix_mul(S, R);
        M = matrix_mul(M, T);

        out.actual = true;
        matrix_copy(M, out.data.arr);
    }

    void decompose_transform_matrix(
            const TransformMatrix& matrix, TransformComponents& out) {
        out.position.x() = matrix.data(0, 3);
        out.position.y() = matrix.data(1, 3);
        out.position.z() = matrix.data(2, 3);

        auto vector_x = vector_init(matrix.data(0, 0), matrix.data(1, 0), matrix.data(2, 0), 0.0f);
        auto vector_y = vector_init(matrix.data(0, 1), matrix.data(1, 1), matrix.data(2, 1), 0.0f);
        auto vector_z = vector_init(matrix.data(0, 2), matrix.data(1, 2), matrix.data(2, 2), 0.0f);

        out.scale.x() = vector_length(vector_x);
        out.scale.y() = vector_length(vector_y);
        out.scale.z() = vector_length(vector_z);

        vector_x /= out.scale.x();
        vector_y /= out.scale.y();
        vector_z /= out.scale.z();

        auto rotation_matrix = matrix_init(
            vector_x, vector_y, vector_z, vector_init(0.0f, 0.0f, 0.0f, 1.0f));
        auto quat = rotation_matrix_to_quaternion(rotation_matrix);
        vector_copy(quat, out.rotation.arr);
    }
}
