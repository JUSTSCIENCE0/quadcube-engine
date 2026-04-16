// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>
#include "configs_implementation.hpp"

namespace QCE {
    class MovementSystem {
    public:
        using Config = MovementConfig;

        explicit MovementSystem(Entities& entities) :
            m_entities(entities)
        {}

        ErrorCode Setup() {
            return ErrorCode::SUCCESS;
        }

        ErrorCode Update();

    private:
        Entities& m_entities;
    };
}
