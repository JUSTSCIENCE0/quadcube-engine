// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/hid/events.hpp>
#include <qce/objects/resource_manager.hpp>

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