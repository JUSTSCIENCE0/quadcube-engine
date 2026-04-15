// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include "configs_implementation.hpp"

#include <qce/ecs/ecs.hpp>
#include <qce/hid/events_handler.hpp>

namespace QCE {
    class HidSystem {
    public:
        using Config = HidConfig;

        explicit HidSystem(Entities&) {}

        ErrorCode Setup(const Config& config);
        ErrorCode Setup();

        void PushEvent(const HidEvent& hid_event);
        void PushMouseMoveEvent(HidEvent hid_event);

        ErrorCode Update();

    private:
        using HidEventsQueue = std::deque<HidEvent>;
        using SingleEventHandlersMap = std::array<std::shared_ptr<BaseCommand>, HidEventCode::E_HEC_COUNT>;

        void ProcessSingleEvents();

        std::optional<HidEvent> m_move_event;
        HidEventsQueue m_events_queue{};
        SingleEventHandlersMap m_single_event_handlers{};
    };
}

