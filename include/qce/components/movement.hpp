// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

namespace QCE {
    struct Movement {
        float3d m_direction{};
        float   m_linear_speed = 0.0f;

        // cache
        mutable bool m_need_recal_velocity = true;
        mutable float3d m_velocity{};
    };

    // TODO: remove Velocity class - migration to ECS
    class Velocity {
    public:
        Velocity(float speed = 0.0f, float3d direction = {}) :
            m_direction(std::move(direction)),
            m_speed(speed) {}

        float3d GetMovement(double time) const {
            float3d result;

            if (m_need_recal) {
                m_need_recal = false;
                vector dir = vector_init(m_direction.arr);
                float dir_len = vector_length(dir);
                if (dir_len > 0.0f) {
                    dir = dir / dir_len;
                    auto vel = dir * m_speed;
                    vector_copy(vel, m_velocity.arr);
                    vector_copy(vel * static_cast<float>(time), result.arr);
                    return result;
                }
                else {
                    m_velocity = {};
                    return {};
                }
            }

            vector vel = vector_init(m_velocity.arr);
            vector_copy(vel * static_cast<float>(time), result.arr);
            return result;
        }

        void SetVelocity(const float3d& velocity) {
            m_velocity = velocity;
            auto dir = vector_init(velocity.arr);
            m_speed = vector_length(dir);
            dir = dir / m_speed;
            vector_copy(dir, m_direction.arr);
        }

        void SetDirection(const float3d& direction) {
            m_direction = direction;
            m_need_recal = true;
        }

        void SetSpeed(float speed) {
            m_speed = speed;
            m_need_recal = true;
        }

        void MoveForward(bool forward) {
            auto dir_z = forward ? 1.0f : -1.0f;

            if (0.0f == m_direction.z()) {
                m_direction.z() = dir_z;
                m_need_recal = true;
                return;
            }

            if (m_direction.z() * dir_z < 0.0f) {
                m_direction.z() = 0.0f;
                m_need_recal = true;
                return;
            }
        }

        void MoveSide(bool left) {
            auto dir_x = left ? -1.0f : 1.0f;

            if (0.0f == m_direction.x()) {
                m_direction.x() = dir_x;
                m_need_recal = true;
                return;
            }

            if (m_direction.x() * dir_x < 0.0f) {
                m_direction.x() = 0.0f;
                m_need_recal = true;
                return;
            }
        }

        void StopForward() {
            m_direction.z() = 0.0f;
            m_need_recal = true;
        }

        void StopSide() {
            m_direction.x() = 0.0f;
            m_need_recal = true;
        }

    private:
        float3d m_direction{};
        float   m_speed = 0.0f;

        // cache
        mutable bool m_need_recal = true;
        mutable float3d m_velocity{};
    };
}
