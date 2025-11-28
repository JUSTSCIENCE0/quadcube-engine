// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/hid/events.hpp>

#include <deque>
#include <optional>
#include <cassert>
#include <memory>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(HidEventType) \
        CU_ENUM_UNIT(E_HET_SINGLE) \
        CU_ENUM_UNIT(E_HET_ACCORD) \
        CU_ENUM_UNIT(E_HET_COMBO) \
    CU_END_ENUM(HidEventType)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

    struct HidEventDescriptor {
        HidEventDescriptor(HidEventType het, const std::string& hname) :
            type(het),
            handler(hname) {}

        const HidEventType type = HidEventType::E_HidEventType_UNKNOWN;
        std::string handler = "";
    };
    struct HidSingleEvent : HidEventDescriptor {
        HidSingleEvent(HidEventCode hec, const std::string& hname) :
            HidEventDescriptor(HidEventType::E_HET_SINGLE, hname),
            code(hec) {}

        HidEventCode code = HidEventCode::E_HEC_END;
    };
    // TODO - HidAccordEvent, HidComboEvent

    class HidEventsManager {
    public:
        using Config = std::vector<std::unique_ptr<HidEventDescriptor>>;

        ErrorCode Setup(const Config& config) {
            for (const auto& descr : config) {
                assert(descr);
                switch (descr->type) {
                case HidEventType::E_HET_SINGLE: {
                    // auto single_descr = static_cast<const HidSingleEvent*>(descr.get());
                    //m_single_event_handlers[single_descr->code] = Get from ResourceManager
                    break;
                }
                case HidEventType::E_HET_ACCORD: {
                    // TODO
                    break;
                }
                case HidEventType::E_HET_COMBO: {
                    // TODO
                    break;
                }
                default: {
                    assert(!"Invalid HidEventType");
                    // TODO - use log system
                    std::cout << "Ignore invalid HidEventType " << descr->type << std::endl;
                    break;
                }
                }
            }

            return ErrorCode::SUCCESS;
        }

        void PushEvent(const HidEvent& hid_event) {
            m_events_queue.push_back(hid_event);
        }
        void PushMouseMoveEvent(HidEvent hid_event) {
            assert(HidEventCode::E_HEC_MOUSE_MOVE == hid_event.GetCode());
            m_move_event.emplace(std::move(hid_event));
        }

        void Process() {
            // TODO - parse & handle events sequence
            ProcessSingleEvents();
        }

    private:
        using HidEventsQueue = std::deque<HidEvent>;
        using SingleEventHandlersMap = std::array<std::unique_ptr<Command>, HidEventCode::E_HEC_COUNT>;

        void ProcessSingleEvents() {
            for (const auto& event : m_events_queue) {
                
                // TODO - handle
                std::cout << hid_event_describe(event) << std::endl;
            }
            m_events_queue.clear();
        }

        std::optional<HidEvent> m_move_event;
        HidEventsQueue m_events_queue{};
        SingleEventHandlersMap m_single_event_handlers{};
    };
}