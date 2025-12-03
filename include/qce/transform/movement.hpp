// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math.hpp>

namespace QCE {
    class Velocity {
    public:
        Velocity(float speed = 0.0f, float3d direction = {}) :
            m_direction(std::move(direction)),
            m_speed(speed) {}

        const float3d& Get() const {
            if (m_need_recal) {
                vector dir = vector_init(m_direction.arr);
                float dir_len = vector_length(dir);
                if (dir_len > 0.0f) {
                    dir = dir / dir_len;
                    vector_copy(dir * m_speed, m_velocity.arr);
                }
                else {
                    m_velocity = {};
                }
            }
            return m_velocity;
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
