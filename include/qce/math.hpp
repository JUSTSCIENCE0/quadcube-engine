// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/vector.hpp>
#include <qce/matrix.hpp>

namespace QCE {
    struct alignas(REQUIRED_ALIGNAS) Vertex {
        vector position;
    };

    struct alignas(REQUIRED_ALIGNAS) Transform {
        vector m_position;
        vector m_rotation; // TODO: quaternion
        vector m_scale;
    };
}
