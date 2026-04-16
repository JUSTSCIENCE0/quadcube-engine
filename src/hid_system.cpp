// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/systems/hid_system.hpp>
#include <qce/ancillary/directories.hpp>

namespace QCE {
    ErrorCode HidSystem::Setup() {
        const auto CONFIGS_DIR = QCE::get_configs_directory();
        const auto hid_events_config_json_file = CONFIGS_DIR / "hid_system.json";

        HidConfig config;
        std::string error_descr = "";
        auto parse_result = macrojson::json_file_to_object(hid_events_config_json_file, config, error_descr);
        if (macrojson::E_MJSON_OK != parse_result) {
            // TODO: use log system
            std::cout << error_descr << std::endl;
            return ErrorCode::E_ENG_BAD_CONFIG_FILE;
        }

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
                auto hid_event_code = hid_event_from_short_name(single_descr->hid_event_code_name.c_str());
                m_single_event_handlers[hid_event_code] =
                    ResourceManager::Get().Read<Command>(single_descr->handler).command;
                if (!m_single_event_handlers[hid_event_code])
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

    void HidSystem::PushEvent(const HidEvent& hid_event) {
        m_events_queue.push_back(hid_event);
    }

    void HidSystem::PushMouseMoveEvent(HidEvent hid_event) {
        assert(HidEventCode::E_HEC_MOUSE_MOVE == hid_event.GetCode());
        m_move_event.emplace(std::move(hid_event));
    }

    ErrorCode HidSystem::Update() {
        // TODO - parse & handle events sequence
        ProcessSingleEvents();

        return ErrorCode::SUCCESS;
    }

    void HidSystem::ProcessSingleEvents() {
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
}
