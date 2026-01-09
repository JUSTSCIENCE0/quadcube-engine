// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>
#include <qce/configs/hid_config.hpp>

namespace QCE {
    class HidSystem {
    public:
        using Config = HidConfig;

        explicit HidSystem(Entities&) {}

        ErrorCode Setup(const Config& config) {
            // register default handler
            QCE_CRITICAL(
                ResourceManager::Get().Add(Command{
                    "HidDescribe",
                    std::make_shared<HidDescribe>()
                })
            );

            for (const auto& descr : config.event_descriptors) {
                assert(descr);
                switch (descr->type) {
                case HidEventType::E_HET_SINGLE: {
                    auto single_descr = static_cast<const HidSingleEvent*>(descr.get());
                    m_single_event_handlers[single_descr->code] =
                        ResourceManager::Get().Read<Command>(single_descr->handler).command;
                    if (!m_single_event_handlers[single_descr->code])
                        return ErrorCode::E_RM_COMMAND_NOT_FOUND;
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

        ErrorCode Update() {
            // TODO - parse & handle events sequence
            ProcessSingleEvents();

            return ErrorCode::SUCCESS;
        }

    private:
        using HidEventsQueue = std::deque<HidEvent>;
        using SingleEventHandlersMap = std::array<std::shared_ptr<BaseCommand>, HidEventCode::E_HEC_COUNT>;

        void ProcessSingleEvents() {
            if (m_single_event_handlers[HidEventCode::E_HEC_MOUSE_MOVE] &&
                m_move_event.has_value()) {
                auto& handler = m_single_event_handlers[HidEventCode::E_HEC_MOUSE_MOVE];
                if (handler)
                    QCE_SOFT(handler->Execute(&m_move_event.value()));
                m_move_event.reset();
            }

            for (const auto& event : m_events_queue) {
                auto& handler = m_single_event_handlers[event.GetCode()];
                if (handler)
                    QCE_SOFT(handler->Execute(&event));
            }
            m_events_queue.clear();
        }

        std::optional<HidEvent> m_move_event;
        HidEventsQueue m_events_queue{};
        SingleEventHandlersMap m_single_event_handlers{};
    };
}

