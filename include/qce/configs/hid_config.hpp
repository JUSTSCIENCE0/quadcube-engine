// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/hid/events_handler.hpp>

namespace QCE {
    struct HidConfig {
        std::vector<
            std::unique_ptr<
                HidEventDescriptor>> event_descriptors{};
    };
}
