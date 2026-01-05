// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>

namespace QCE {
    class MovementSystem {
    public:
        explicit MovementSystem(Entities& entities) :
            m_entities(entities)
        {}
        ErrorCode Update();

    private:
        Entities& m_entities;
    };
}
