// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD sse2
#endif // !CU_BUILD_SPECIFIC_SIMD

#include <qce/components/camera.hpp>

#include <iostream>

namespace QCE {
    static inline void camera_load_params(
            CameraView& camera,
            vector& position,
            vector& target,
            vector& forward,
            vector& right,
            vector& up_real) {
        target   = vector_init(camera.target.arr);
        position = vector_init(camera.position.arr);

        if (!camera.actual_fru) {
            auto up = vector_init(camera.up_direction.arr);

            forward = vector_normalize(target - position);
            right   = vector_normalize(vector_cross_product(up, forward));
            up_real = vector_cross_product(forward, right);

            vector_copy(forward, camera.forward.arr);
            camera.actual_yaw_pitch = false;

            vector_copy(right,   camera.right.arr);
            vector_copy(up_real, camera.actual_up.arr);

            camera.actual_fru = true;
            camera.actual_view = false;
        }
        else {
            forward = vector_init(camera.forward.arr);
            right   = vector_init(camera.right.arr);
            up_real = vector_init(camera.actual_up.arr);
        }
    }

    void camera_recalc_view(CameraView& camera) {
        assert(camera.is_LH && "Only LH implemented");

        vector position, target, forward, right, up_real;
        camera_load_params(camera, position, target, forward, right, up_real);

        auto view = camera_look_to_lh_matrix(position, forward, right, up_real);
        matrix_copy(view, camera.view.arr);
        camera.actual_view = true;
    }

    void camera_recalc_proj(CameraProj& camera) {
        assert(camera.is_LH && "Only LH implemented");

        float height = 1.0f / (std::tanf(camera.fov_rad * 0.5f));
        float width = height / camera.aspect;
        float range_z = camera.zfar / (camera.zfar - camera.znear);

        camera.proj(0, 0) = width;
        camera.proj(1, 1) = height;
        camera.proj(2, 2) = range_z;
        camera.proj(3, 2) = -1.0f * camera.znear * range_z;
        camera.actual_proj = true;
    }

    void camera_move(CameraView& camera, const float3d& move) {
        if (!move.x() && !move.z())
            return;

        vector position, target, forward, right, up_real;
        camera_load_params(camera, position, target, forward, right, up_real);

        auto delta = (forward * move.z()) + (right * move.x());
        position += delta;
        vector_copy(position, camera.position.arr);

        target += delta;
        vector_copy(target, camera.target.arr);

        camera.actual_view = false;
    }

    void camera_rotate_view(CameraView& camera, float d_yaw, float d_pitch) {
        vector position, target, forward, right, up_real;
        camera_load_params(camera, position, target, forward, right, up_real);

        if (!camera.actual_yaw_pitch) {
            assert(camera.actual_fru);

            camera.pitch = asinf(camera.forward.y());
            camera.yaw = atan2(camera.forward.x(), camera.forward.z());

            camera.actual_yaw_pitch = true;
        }

        camera.pitch += d_pitch;
        if (camera.pitch > camera.max_pitch_abs)
            camera.pitch = camera.max_pitch_abs;
        if (camera.pitch < -camera.max_pitch_abs)
            camera.pitch = -camera.max_pitch_abs;

        camera.yaw += d_yaw;

        camera.forward.x() = cosf(camera.pitch) * sinf(camera.yaw);
        camera.forward.y() = sinf(camera.pitch);
        camera.forward.z() = cosf(camera.pitch) * cosf(camera.yaw);

        forward = vector_init(camera.forward.arr);
        forward = vector_normalize(forward);

        float dist = vector_length(target - position);
        target = position + (forward * dist);
        vector_copy(target, camera.target.arr);

        camera.actual_fru = false;
    }
}