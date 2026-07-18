// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>

struct TransformAnimationConfig {};

namespace QCE {
    class TransformAnimationSystem {
    public:
        using Config = TransformAnimationConfig;

        explicit TransformAnimationSystem(Entities& entities) :
            m_entities(entities) {}

        ErrorCode Setup() {
            return ErrorCode::SUCCESS;
        }

        ErrorCode Update();

    private:
        Entities& m_entities;
    };
}
