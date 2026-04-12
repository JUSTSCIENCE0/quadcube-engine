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