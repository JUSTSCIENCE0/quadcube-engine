// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include "configs_implementation.hpp"

#include <qce/hid/events.hpp>
#include <qce/objects/resource_manager.hpp>

#include <deque>
#include <optional>
#include <cassert>
#include <memory>

namespace QCE {
    struct HidEventDescriptor {
        HidEventDescriptor(HidEventType het, const std::string& hname) :
            type(het),
            handler(hname) {}

        const HidEventType type = HidEventType::E_HET_UNKNOWN;
        std::string handler = "";
    };
    struct HidSingleEvent : HidEventDescriptor {
        HidSingleEvent(HidEventCode hec, const std::string& hname) :
            HidEventDescriptor(HidEventType::E_HET_SINGLE, hname),
            code(hec) {}

        HidEventCode code = HidEventCode::E_HEC_END;
    };
    // TODO - HidAccordEvent, HidComboEvent

    struct HidDescribe : BaseCommand {
        HidDescribe() :
            BaseCommand("HidDescribe") {}
        ErrorCode Execute(const CommandContext* context) override {
            assert(context);
            assert(context->type == CommandContextType::E_CCT_HID_EVENT);
            auto hid_event = static_cast<const HidEvent*>(context);

            // TODO - use log system
            std::cout << hid_event_describe(*hid_event) << std::endl;
            return ErrorCode::SUCCESS;
        }
    };
}