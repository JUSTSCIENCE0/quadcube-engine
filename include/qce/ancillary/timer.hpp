// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <chrono>
#include <concepts>
#include <stdint.h>

namespace QCE {
    template<typename T>
    concept TimerRatio =
        std::same_as<T, std::ratio<1, 1>> ||
        std::same_as<T, std::milli> ||
        std::same_as<T, std::micro>;

    class Timer {
    public:
        using clock = std::chrono::steady_clock;
        using timepoint = clock::time_point;

        void Update(timepoint curr = clock::now()) noexcept {
            m_prev_time = m_curr_time;
            m_curr_time = curr;
        }

        bool Check(double timeout_sec, bool update = true) noexcept {
            using namespace std::chrono;

            auto curr = clock::now();
            auto delta = duration_cast<seconds>(curr - m_curr_time);
            auto delta_sec = duration<double>(delta).count();
            bool is_elapsed = delta_sec >= timeout_sec;
            if (is_elapsed && update)
                Update(curr);
            return is_elapsed;
        }

        template <TimerRatio DeltaRatio = std::ratio<1, 1>>
        double GetDelta() const noexcept {
            using namespace std::chrono;

            return duration<double, DeltaRatio>(m_curr_time - m_prev_time).count();
        }

        int64_t GetDeltaNs() const noexcept {
            return (m_curr_time - m_prev_time).count();
        }

    private:
        timepoint m_curr_time = clock::now();
        timepoint m_prev_time = clock::now();
    };

    class FrameTime {
    public:
        FrameTime(const FrameTime&) = delete;
        FrameTime(FrameTime&&) = delete;
        FrameTime& operator=(const FrameTime&) = delete;
        FrameTime& operator=(FrameTime&&) = delete;

        static FrameTime& Get() {
            static FrameTime instance{};
            return instance;
        }

        void NextFrame() noexcept {
            m_timer.Update();
        }

        template <TimerRatio DeltaRatio = std::ratio<1, 1>>
        double Elapsed() const noexcept {
            return m_timer.GetDelta<DeltaRatio>();
        }

        int64_t ElapsedNs() const noexcept {
            return m_timer.GetDeltaNs();
        }

    private:
        FrameTime() = default;

        Timer m_timer{};
    };
}
