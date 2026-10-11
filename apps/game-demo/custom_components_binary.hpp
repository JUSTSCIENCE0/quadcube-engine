// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include "custom_components.hpp"

#include <qce/loaders/binary/resources.hpp>

template<typename S>
void serialize(S& s, DeformationDescription& description) {
    s.value4b(description.update_period_sec);
    s.object(description.plane_params);
    s.value1b(description.is_reflected);
}
