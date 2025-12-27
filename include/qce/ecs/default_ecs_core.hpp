// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/entity_manager.hpp>
#include <qce/ecs/systems_hub.hpp>

namespace QCE {
    using Entities = EntityManager<
        64 // Cache Limit
        // TODO: Components List
    >;
}
