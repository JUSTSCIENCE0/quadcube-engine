// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/hid/events.hpp>

#include <deque>
#include <optional>
#include <cassert>

namespace QCE {
    class HidEventsManager {
    public:
        void PushEvent(const HidEvent& hid_event) {
            m_events_queue.push_back(hid_event);
        }
        void PushMouseMoveEvent(const HidEvent& hid_event) {
            assert(HidEventCode::E_HEC_MOUSE_MOVE == hid_event.GetCode());
            m_move_event = hid_event;
        }

        void Process() {
            // TODO - parse & handle events sequence
            ProcessSingleEvents();
        }

    private:
        using HidEventsQueue = std::deque<HidEvent>;

        void ProcessSingleEvents() {
            for (const auto& event : m_events_queue) {
                // TODO - handle
                std::cout << hid_event_describe(event) << std::endl;
            }
            m_events_queue.clear();
        }

        std::optional<HidEvent> m_move_event;
        HidEventsQueue m_events_queue{};
    };
}