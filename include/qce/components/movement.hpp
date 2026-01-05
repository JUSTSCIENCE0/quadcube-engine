// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

namespace QCE {
    struct Movement {
        float3d direction{};
        float   linear_speed = 0.0f;

        bool    need_reset_direction = false;

        // cache
        bool    actual_velocity = false;
        float3d linear_velocity{};
    };

    // helpers
    static inline float3d movement_calc_translation(const Movement& movement, double time) {
        vector vel = vector_init(movement.linear_velocity.arr);
        float3d result;
        vector_copy(vel * static_cast<float>(time), result.arr);
        return result;
    }

    static inline void movement_recalc_velocity(Movement& movement) {
        vector dir = vector_init(movement.direction.arr);
        float dir_len = vector_length(dir);
        if (dir_len > 0.0f) {
            dir = dir / dir_len;
            auto vel = dir * movement.linear_speed;
            vector_copy(vel, movement.linear_velocity.arr);
        }
        else {
            movement.linear_velocity = {};
        }

        movement.actual_velocity = true;
    }

    static inline void movement_apply_forward(Movement& movement, bool forward) {
        auto dir_z = forward ? 1.0f : -1.0f;

        if (0.0f == movement.direction.z()) {
            movement.direction.z() = dir_z;
            movement.actual_velocity = false;
            return;
        }

        if (movement.direction.z() * dir_z < 0.0f) {
            movement.direction.z() = 0.0f;
            movement.actual_velocity = false;
            return;
        }
    }

    static inline void movement_apply_side(Movement& movement, bool left) {
        auto dir_x = left ? -1.0f : 1.0f;

        if (0.0f == movement.direction.x()) {
            movement.direction.x() = dir_x;
            movement.actual_velocity = false;
            return;
        }

        if (movement.direction.x() * dir_x < 0.0f) {
            movement.direction.x() = 0.0f;
            movement.actual_velocity = false;
            return;
        }
    }

    static inline void movement_stop_forward(Movement& movement) {
        movement.direction.z() = 0.0f;
        movement.actual_velocity = false;
    }

    static inline void movement_stop_side(Movement& movement) {
        movement.direction.x() = 0.0f;
        movement.actual_velocity = false;
    }

    static inline void movement_calc_from_velocity(Movement& movement, const float3d& velocity) {
        movement.linear_velocity = velocity;
        auto dir = vector_init(velocity.arr);
        movement.linear_speed = vector_length(dir);
        dir = dir / movement.linear_speed;
        vector_copy(dir, movement.direction.arr);
        movement.actual_velocity = true;
    }
}
